#include "stdafx.h"
#include "LightSettings.h"

#include <QtWidgets/QPushButton>
#include <QtWidgets/QColorDialog>
#include <QtCore/QTimer>

#include <osg/LightModel>
#include <osgViewer/View>

#include "ui_LightSettings.h"

#include <sgi/plugins/SGIItemOsg>
#include <sgi/helpers/osg>

#include <osgEarth/Version>
#include <osgEarthUtil/Sky>

#include <osg/io_utils>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

inline osg::Vec4 osgColor(const QColor & color)
{
    return osg::Vec4(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}

inline QColor osgColor(const osg::Vec4 & color)
{
    QColor ret;
    ret.setRgbF(color.x(), color.y(), color.z(), color.w());
    return ret;
}

void skySetExposure(osgEarth::Util::SkyNode * sky, float exposure)
{
    osg::StateSet * stateset = sky->getStateSet();
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
    osg::Uniform * uniform = stateset->getUniform("atmos_exposure");
#else
    osg::Uniform * uniform = stateset->getUniform("oe_sky_exposure");
#endif
    if (uniform)
        uniform->set(exposure);
}

float skyGetExposure(osgEarth::Util::SkyNode * sky)
{
    float ret = 0.0f;
    osg::StateSet * stateset = sky->getStateSet();
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
    osg::Uniform * uniform = stateset->getUniform("atmos_exposure");
#else
    osg::Uniform * uniform = stateset->getUniform("oe_sky_exposure");
#endif
    if (uniform)
        uniform->get(ret);
    return ret;
}

/// @brief returns the current ambient daylight boost
/// @return current ambient daylight boost
float skyGetAmbientDaylightBoost(osgEarth::Util::SkyNode * sky)
{
    float ret = 0.0f;
    osg::StateSet * stateset = sky->getStateSet();
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
    osg::Uniform * uniform = stateset->getUniform("atmos_boost");
#else
    osg::Uniform * uniform = stateset->getUniform("oe_sky_ambientBoostFactor");
#endif
    if (uniform)
        uniform->get(ret);
    return ret;
}

/// @brief sets the ambient daylight boost factor
/// @param factor new boost factor; 0 is off, 2-10 is reasonable
/// @param save true to save the settings persistently
void skySetAmbientDaylightBoost(osgEarth::Util::SkyNode * sky, float factor)
{
    osg::StateSet * stateset = sky->getStateSet();
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
    osg::Uniform * uniform = stateset->getUniform("atmos_boost");
#else
    osg::Uniform * uniform = stateset->getUniform("oe_sky_ambientBoostFactor");
#endif
    if (uniform)
        uniform->set(factor);
}

LightSettings::LightSettings(QWidget * parent, SGIPluginHostInterface * hostInterface, SGIItemBase * item, ISettingsDialogInfo * info)
	: SettingsQDialogImpl(parent, hostInterface, item, info)
	, m_sky(nullptr)
	, m_ready(false)
    , m_updateTimer(new QTimer)
{
    initialize();
}

LightSettings::~LightSettings()
{
	delete ui;
    delete m_updateTimer;
}

void LightSettings::initialize()
{
    m_sky = getObject<osgEarth::Util::SkyNode, SGIItemOsg, DynamicCaster>();
    if (m_sky)
    {
        osg::Camera * camera = osg_helpers::findCamera(m_sky);
        if (camera)
        {
            osg::View * view = camera->getView();
            if (view)
                m_views.insert(view);
        }
    }
    else
    {
        m_camera = getObject<osg::Camera, SGIItemOsg, DynamicCaster>();
        if (m_camera)
        {
            osg::LightSource * lightSource = osg_helpers::findTopMostNodeOfType<osg::LightSource>(m_camera);
            m_light = lightSource ? lightSource->getLight() : nullptr;
            osg::View * view = m_camera->getView();
            if (view)
                m_views.insert(view);
            if(!m_light)
                m_light = view->getLight();
        }
    }

    ui = new Ui_LightSettings;
    ui->setupUi( this );

    ui->lightingMode->blockSignals(true);
    ui->lightingMode->addItem("No light", QVariant(osg::View::NO_LIGHT));
    ui->lightingMode->addItem("Headlight", QVariant(osg::View::HEADLIGHT));
    ui->lightingMode->addItem("Sky light", QVariant(osg::View::SKY_LIGHT));
    ui->lightingMode->blockSignals(false);

    ui->skyMode->blockSignals(true);
#if 0
    ui->skyMode->addItem("Advance date/time", QVariant(SceneGraphSky::ModeAdvanceDateTime));
    ui->skyMode->addItem("Fixed date/time", QVariant(SceneGraphSky::ModeFixedDateTime));
    ui->skyMode->addItem("Use camera position", QVariant(SceneGraphSky::ModeUseCurrentCameraPosition));
    ui->skyMode->addItem("Fixed position", QVariant(SceneGraphSky::ModeFixedPosition));
    ui->skyMode->addItem("Use current date/time", QVariant(SceneGraphSky::ModeUseCurrentDateTime));
    ui->skyMode->addItem("Solar noon", QVariant(SceneGraphSky::ModeSolarNoon));
    ui->skyMode->addItem("Fixed time at noon", QVariant(SceneGraphSky::ModeFixedTimeNoon));
#endif
    ui->skyMode->blockSignals(false);

    connect(ui->buttonBox->button(QDialogButtonBox::Save), &QPushButton::clicked, this, &LightSettings::save);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, [this] { this->apply(false); } );
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &LightSettings::reject);
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, this, &LightSettings::reset);
    connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this, &LightSettings::restoreDefaults);

    connect(m_updateTimer, &QTimer::timeout, this, &LightSettings::updateFromSky);

    load();
    m_ready = true;
}

void LightSettings::showEvent ( QShowEvent * event )
{
	load();

	QDialog::showEvent(event);
}

void LightSettings::setColorToLineEdit(QLineEdit * edit, const QColor & color)
{
	char sz[64];
	sprintf(sz, "%0.2ff, %0.2ff, %0.2ff, %0.2ff", color.redF(), color.greenF(), color.blueF(), color.alphaF());

    QColor textColor = QColor::fromRgbF(1.0f - color.redF(), 1.0f - color.greenF(), 1.0f - color.blueF(), 1.0f);

    QPalette palette(edit->palette());
    palette.setColor(QPalette::Base, color);
    palette.setColor(QPalette::Text, textColor);
    edit->blockSignals(true);
    edit->setText(QString(sz));
    edit->setPalette(palette);
    edit->blockSignals(false);
}

void LightSettings::reset()
{
//     m_sky->reset();
    load();
}

void LightSettings::load()
{
    int index;
    bool globalLightingEnabled = false;
    osg::View::LightingMode lightingMode = osg::View::NO_LIGHT;
    int skymode = -1;
    //SceneGraphSky::Mode skymode = SceneGraphSky::ModeInvalid;
    bool atmosphericLighting = false;
    if(m_sky)
    {
        globalLightingEnabled = m_sky->getLighting() &  osg::StateAttribute::ON;
        atmosphericLighting = globalLightingEnabled;
//         lightingMode = m_sky->lightingMode();
//         skymode = m_sky->mode();
    }
    else
    {
        osgEarth::DateTime dt(2016, 8, 10, 14.0);

        osgEarth::Util::Ephemeris e;
        osgEarth::Util::CelestialBody sun = e.getSunPosition(dt);
        osg::Vec3d world = sun.geocentric;
        world.normalize();

        std::stringstream ss;
        ss << world;
        ss.flush();

        QDateTime dateTime = QDateTime::fromSecsSinceEpoch(dt.asTimeStamp(), Qt::UTC);
        QString sunPosition = QString::fromStdString(ss.str());
        QString moonPosition;

        ui->dateEdit->blockSignals(true);
        ui->timeEdit->blockSignals(true);
        ui->sunPosition->blockSignals(true);
        ui->moonPosition->blockSignals(true);

        ui->dateEdit->setDateTime(dateTime);
        ui->timeEdit->setDateTime(dateTime);
        ui->sunPosition->setText(sunPosition);
        ui->moonPosition->setText(moonPosition);

        ui->dateEdit->blockSignals(false);
        ui->timeEdit->blockSignals(false);
        ui->sunPosition->blockSignals(false);
        ui->moonPosition->blockSignals(false);

        for (osg::View * view : m_views)
        {
            const osg::StateSet * stateSet = view->getCamera()->getStateSet();
            if (stateSet)
            {
#if defined(OSG_GL3_AVAILABLE) && !defined(OSG_GL_FIXED_FUNCTION_AVAILABLE)
                globalLightingEnabled = view->getLight() != nullptr;
#else
                globalLightingEnabled = (stateSet->getMode(GL_LIGHTING) & osg::StateAttribute::ON);
#endif
            }
            else
                globalLightingEnabled = true;
            lightingMode = view->getLightingMode();
        }
        //skymode = SceneGraphSky::ModeFixedDateTime;
    }
    ui->groupBoxLight->blockSignals(true);
	ui->groupBoxLight->setChecked(globalLightingEnabled);
    ui->groupBoxLight->blockSignals(false);

    ui->atmosphericLighting->blockSignals(true);
    ui->atmosphericLighting->setChecked(atmosphericLighting);
    ui->atmosphericLighting->blockSignals(false);

    float ambientIntensity = 1.0f;
    float ambientBrightness = 1.0f;
    float exposure = 1.0f;
    float daylightBoost = 1.0f;
    float timeFactor = 1.0f;
    if(m_sky)
    {
        osg::Light* light = m_sky->getSunLight();
        m_ambientLightColor = light ? osgColor(light->getAmbient()) : QColor();
        m_diffuseLightColor = light ? osgColor(light->getDiffuse()) : QColor();
        m_specularLightColor = light ? osgColor(light->getSpecular()) : QColor();

// 	    ambientIntensity = m_sky->ambientLightIntensity();
//         timeFactor = m_sky->timeFactor();
        exposure = skyGetExposure(m_sky);
        daylightBoost = skyGetAmbientDaylightBoost(m_sky);
    }
    else
    {
        for (osg::View * view : m_views)
        {
            const osg::Camera * camera = view->getCamera();
            const osg::StateSet * stateSet = camera->getStateSet();
            const osg::LightModel* lightmodel = stateSet ? dynamic_cast<const osg::LightModel*>(stateSet->getAttribute(osg::StateAttribute::LIGHTMODEL)) : NULL;

            osg::Light* light = view->getLight();

            ambientIntensity = lightmodel ? lightmodel->getAmbientIntensity().x() : 0.0f;
            m_ambientLightColor = light ? osgColor(light->getAmbient()) : QColor();
            m_diffuseLightColor = light ? osgColor(light->getDiffuse()) : QColor();
            m_specularLightColor = light ? osgColor(light->getSpecular()) : QColor();
        }
    }
    ui->ambientBrightness->blockSignals(true);
    ui->ambientBrightnessSpinBox->blockSignals(true);
    ui->ambientBrightness->setValue(ambientBrightness * 100.0f);
    ui->ambientBrightnessSpinBox->setValue(ambientBrightness);
    ui->ambientBrightness->blockSignals(false);
    ui->ambientBrightnessSpinBox->blockSignals(false);

    ui->daytimeAmbientBoost->blockSignals(true);
    ui->daytimeAmbientBoostSpinBox->blockSignals(true);
    ui->daytimeAmbientBoost->setValue(daylightBoost * 100.0f);
    ui->daytimeAmbientBoostSpinBox->setValue(daylightBoost);
    ui->daytimeAmbientBoost->setEnabled(m_sky != nullptr);
    ui->daytimeAmbientBoostSpinBox->setEnabled(m_sky != nullptr);
    ui->daytimeAmbientBoost->blockSignals(false);
    ui->daytimeAmbientBoostSpinBox->blockSignals(false);

    ui->ambientIntensity->blockSignals(true);
    ui->ambientIntensitySpinBox->blockSignals(true);
    ui->ambientIntensity->setValue(ambientIntensity * 100.0f);
    ui->ambientIntensitySpinBox->setValue(ambientIntensity);
    ui->ambientIntensity->blockSignals(false);
    ui->ambientIntensitySpinBox->blockSignals(false);

    ui->exposure->blockSignals(true);
    ui->exposureSpinBox->blockSignals(true);
    ui->exposure->setValue(exposure * 100.0f);
    ui->exposureSpinBox->setValue(exposure);
    ui->exposure->setEnabled(m_sky != nullptr);
    ui->exposureSpinBox->setEnabled(m_sky != nullptr);
    ui->exposure->blockSignals(false);
    ui->exposureSpinBox->blockSignals(false);

    ui->timeFactor->blockSignals(true);
    ui->timeFactor->setValue(timeFactor);
    ui->timeFactor->blockSignals(false);

	index = ui->lightingMode->findData(QVariant(lightingMode));
    ui->lightingMode->blockSignals(true);
	ui->lightingMode->setCurrentIndex(index);
    ui->lightingMode->blockSignals(false);

    index = ui->skyMode->findData(QVariant(skymode));
    ui->skyMode->blockSignals(true);
    ui->skyMode->setCurrentIndex(index);
    ui->skyMode->blockSignals(false);
    ui->skyMode->setEnabled((m_sky != NULL));
    enableSkyModeParameterControls();

    ui->atmosphereVisible->blockSignals(true);
    if (m_sky)
        ui->atmosphereVisible->setChecked(m_sky->getAtmosphereVisible());
    else
        ui->atmosphereVisible->setChecked(false);
    ui->atmosphereVisible->setEnabled(m_sky != nullptr);
    ui->atmosphereVisible->blockSignals(false);
    ui->sunVisible->blockSignals(true);
    if (m_sky)
        ui->sunVisible->setChecked(m_sky->getSunVisible());
    else
        ui->sunVisible->setChecked(false);
    ui->sunVisible->setEnabled(m_sky != nullptr);
    ui->sunVisible->blockSignals(false);
    ui->moonVisible->blockSignals(false);
    if (m_sky)
        ui->moonVisible->setChecked(m_sky->getMoonVisible());
    else
        ui->moonVisible->setChecked(false);
    ui->moonVisible->setEnabled(m_sky != nullptr);
    ui->moonVisible->blockSignals(false);
    ui->starsVisible->blockSignals(false);
    if (m_sky)
        ui->starsVisible->setChecked(m_sky->getStarsVisible());
    else
        ui->starsVisible->setChecked(false);
    ui->starsVisible->setEnabled(m_sky != nullptr);
    ui->starsVisible->blockSignals(false);

	setColorToLineEdit(ui->diffuseLightColorText, m_diffuseLightColor);
	setColorToLineEdit(ui->specularLightColorText, m_specularLightColor);

    updateFromSky();
    enableSkyModeParameterControls();

    m_updateTimer->setInterval(1000);
    m_updateTimer->start();
}

void LightSettings::save()
{
	apply(true);
	accept();
}

void LightSettings::apply()
{
	apply(false);
}

void LightSettings::apply(bool save)
{
	if(!m_ready)
		return;
}

void LightSettings::restoreDefaults()
{
	load();
}

void LightSettings::reject()
{
	QDialog::reject();
}

void LightSettings::changeDiffuseLightColor()
{
	QColor newColor = QColorDialog::getColor(m_diffuseLightColor, this, tr("Select Diffuse Color"), QColorDialog::ShowAlphaChannel|QColorDialog::DontUseNativeDialog);
	if(newColor.isValid())
	{
		m_diffuseLightColor = newColor;
        setColorToLineEdit(ui->diffuseLightColorText, m_diffuseLightColor);
        if(m_sky)
            m_sky->getSunLight()->setDiffuse(osgColor(m_diffuseLightColor));
        else if(m_light)
            m_light->setDiffuse(osgColor(m_diffuseLightColor));
        triggerRepaint();
	}
}

void LightSettings::changeSpecularLightColor()
{
	QColor newColor = QColorDialog::getColor(m_specularLightColor, this, tr("Select Specular Color"), QColorDialog::ShowAlphaChannel|QColorDialog::DontUseNativeDialog);
	if(newColor.isValid())
	{
		m_specularLightColor = newColor;
        setColorToLineEdit(ui->specularLightColorText, m_specularLightColor);
        if (m_sky)
            m_sky->getSunLight()->setSpecular(osgColor(m_specularLightColor));
        else if(m_light)
            m_light->setSpecular(osgColor(m_specularLightColor));
        triggerRepaint();
	}
}

void LightSettings::skyModeChanged(int index)
{
//     SceneGraphSky::Mode skymode = SceneGraphSky::ModeInvalid;
//     if(index >= 0)
//     {
//         skymode = (SceneGraphSky::Mode)ui->skyMode->itemData(index).toInt();
//         m_sky->setMode(skymode, false);
//     }
    enableSkyModeParameterControls();
    triggerRepaint();
}

void LightSettings::enableSkyModeParameterControls()
{
    int skymode = -1;
//     SceneGraphSky::Mode skymode = SceneGraphSky::ModeInvalid;
//     int index = ui->skyMode->currentIndex();
//     if(index >= 0)
//         skymode = (SceneGraphSky::Mode)ui->skyMode->itemData(index).toInt();

    ui->lightingMode->setEnabled(m_sky == nullptr);
    ui->ambientIntensity->setEnabled(m_sky == nullptr);
    ui->ambientIntensitySpinBox->setEnabled(m_sky == nullptr);

    ui->dateEdit->blockSignals(true);
    ui->timeEdit->blockSignals(true);
    ui->sunPosition->blockSignals(true);
    ui->moonPosition->blockSignals(true);
    ui->timeFactor->blockSignals(true);

    ui->dateEdit->setEnabled(m_sky != nullptr || m_light != nullptr);
    ui->timeEdit->setEnabled(m_sky != nullptr || m_light != nullptr);
    ui->sunPosition->setEnabled(m_sky != nullptr || m_light != nullptr);
    ui->moonPosition->setEnabled(m_sky != nullptr);
    ui->timeFactor->setEnabled(m_sky != nullptr);

    QDateTime dateTime;
    QString sunPosition;
    QString moonPosition;
    float timeFactor = 1.0f;

    if (m_sky)
    {
        //         GpsCoordinate sunPositionGPS = m_sky->getSunPosition();
        //         GpsCoordinate moonPositionGPS = m_sky->getMoonPosition();
        //         dateTime = m_sky->getDateTime();
        //         sunPosition = sunPositionGPS.toString().toQString();
        //         moonPosition = moonPositionGPS.toString().toQString();
        //         timeFactor = m_sky->timeFactor();


        ui->dateEdit->setDateTime(dateTime);
        ui->timeEdit->setDateTime(dateTime);
        ui->sunPosition->setText(sunPosition);
        ui->moonPosition->setText(moonPosition);
        ui->timeFactor->setValue(timeFactor);
    }

#if 0
    switch(skymode)
    {
    case SceneGraphSky::ModeInvalid:
        ui->dateEdit->setEnabled(false);
        ui->timeEdit->setEnabled(false);
        ui->sunPosition->setEnabled(false);
        ui->moonPosition->setEnabled(false);
        ui->timeFactor->setEnabled(false);
        break;
    case SceneGraphSky::ModeAdvanceDateTime:
        ui->dateEdit->setEnabled(false);
        ui->timeEdit->setEnabled(false);
        ui->sunPosition->setEnabled(false);
        ui->moonPosition->setEnabled(false);
        ui->timeFactor->setEnabled(true);
        break;
    case SceneGraphSky::ModeFixedDateTime:
        ui->dateEdit->setEnabled(true);
        ui->timeEdit->setEnabled(true);
        ui->sunPosition->setEnabled(false);
        ui->moonPosition->setEnabled(false);
        ui->timeFactor->setEnabled(false);
        break;
    case SceneGraphSky::ModeUseCurrentCameraPosition:
        ui->dateEdit->setEnabled(false);
        ui->timeEdit->setEnabled(false);
        ui->sunPosition->setEnabled(false);
        ui->moonPosition->setEnabled(false);
        ui->timeFactor->setEnabled(false);
        break;
    case SceneGraphSky::ModeFixedPosition:
        ui->dateEdit->setEnabled(false);
        ui->timeEdit->setEnabled(false);
        ui->sunPosition->setEnabled(true);
        ui->moonPosition->setEnabled(true);
        ui->timeFactor->setEnabled(false);
        break;
    case SceneGraphSky::ModeUseCurrentDateTime:
        ui->dateEdit->setEnabled(false);
        ui->timeEdit->setEnabled(false);
        ui->sunPosition->setEnabled(false);
        ui->moonPosition->setEnabled(false);
        ui->timeFactor->setEnabled(false);
        break;
    case SceneGraphSky::ModeSolarNoon:
        ui->dateEdit->setEnabled(false);
        ui->timeEdit->setEnabled(false);
        ui->sunPosition->setEnabled(false);
        ui->moonPosition->setEnabled(false);
        ui->timeFactor->setEnabled(false);
        break;
    case SceneGraphSky::ModeFixedTimeNoon:
        ui->dateEdit->setEnabled(false);
        ui->timeEdit->setEnabled(false);
        ui->sunPosition->setEnabled(false);
        ui->moonPosition->setEnabled(false);
        ui->timeFactor->setEnabled(false);
        break;
    }
#endif
    ui->dateEdit->blockSignals(false);
    ui->timeEdit->blockSignals(false);
    ui->sunPosition->blockSignals(false);
    ui->moonPosition->blockSignals(false);
    ui->timeFactor->blockSignals(false);
}

void LightSettings::setAtmosphereVisible(bool visible)
{
    if(m_sky)
        m_sky->setAtmosphereVisible(visible);
    triggerRepaint();
}

void LightSettings::setSunVisible(bool visible)
{
    if(m_sky)
        m_sky->setSunVisible(visible);
    triggerRepaint();
}

void LightSettings::setMoonVisible(bool visible)
{
    if(m_sky)
        m_sky->setMoonVisible(visible);
    triggerRepaint();
}

void LightSettings::setStarsVisible(bool visible)
{
    if(m_sky)
        m_sky->setStarsVisible(visible);
    triggerRepaint();
}

void LightSettings::setAtmosphericLighting(bool enable)
{
    m_sky->setLighting(enable ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
    triggerRepaint();
}

void LightSettings::setSunPosition(const QString & position)
{
    if (m_sky)
    {
    }
    else if(m_light)
    {
        std::string s(position.toStdString());
        std::stringstream ss(s);
        osg::Vec3d world;
        ss >> world;

        world.normalize();
        m_light->setPosition(osg::Vec4d(world, 0.0));
    }

//     bool ok = false;
//     GpsCoordinate gpspos = GpsCoordinate::fromString(position, &ok);
//     if(ok)
//     {
//         m_sky->setSunPosition(gpspos);
//         triggerRepaint();
//     }
}

void LightSettings::setMoonPosition(const QString & position)
{
//     bool ok = false;
//     GpsCoordinate gpspos = GpsCoordinate::fromString(position, &ok);
//     if(ok)
//     {
//         m_sky->setMoonPosition(gpspos);
//         triggerRepaint();
//     }
}

void LightSettings::setDateTime(const QDateTime & datetime)
{
    QDateTime utc = datetime.toUTC();
    osgEarth::DateTime dt(utc.toSecsSinceEpoch());
    if(m_sky)
        m_sky->setDateTime(dt);
    else if (m_light)
    {
        osgEarth::Util::Ephemeris e;
        osgEarth::Util::CelestialBody sun = e.getSunPosition(dt);
        osg::Vec3d world = sun.geocentric;

        world.normalize();
        m_light->setPosition(osg::Vec4d(world, 0.0));
    }

    triggerRepaint();
}

void LightSettings::lightingModeChanged(int index)
{
    if(index >= 0)
    {
        osg::View::LightingMode lightingMode = (osg::View::LightingMode)ui->lightingMode->itemData(index).toInt();
        for (osg::View * view : m_views)
            view->setLightingMode(lightingMode);
        triggerRepaint();
    }
}

void LightSettings::ambientBrightnessChanged(int value)
{
    float real_value = (float)value / 100.0f;
    ui->ambientBrightnessSpinBox->blockSignals(true);
    ui->ambientBrightnessSpinBox->setValue(real_value);
    ui->ambientBrightnessSpinBox->blockSignals(false);
    if(m_sky)
        m_sky->getSunLight()->setAmbient(osg::Vec4(real_value, real_value, real_value, 1.0f));
    else if(m_light)
        m_light->setAmbient(osg::Vec4(real_value, real_value, real_value, 1.0f));
    triggerRepaint();
}

void LightSettings::ambientBrightnessChanged(double value)
{
    int int_value = (int)osg::round(value * 100.0);
    ui->ambientBrightness->blockSignals(true);
    ui->ambientBrightness->setValue(int_value);
    ui->ambientBrightness->blockSignals(false);
    if(m_sky)
        m_sky->getSunLight()->setAmbient(osg::Vec4(value, value, value, 1.0f));
    else if(m_light)
        m_light->setAmbient(osg::Vec4(value, value, value, 1.0f));
    triggerRepaint();
}

void LightSettings::ambientIntensityChanged(int value)
{
    float real_value = (float)value / 100.0f;
    ui->ambientIntensitySpinBox->blockSignals(true);
    ui->ambientIntensitySpinBox->setValue(real_value);
    ui->ambientIntensitySpinBox->blockSignals(false);
    ambientIntensityChanged(real_value);
}

void LightSettings::ambientIntensityChanged(double value)
{
    int int_value = (int)osg::round(value * 100.0);
    ui->ambientIntensity->blockSignals(true);
    ui->ambientIntensity->setValue(int_value);
    ui->ambientIntensity->blockSignals(false);

    if(m_sky)
    {
//         m_sky->setAmbientLightIntensity(value, false);
    }
    else
    {
        for (osg::View * view : m_views)
        {
            osg::Camera * camera = view->getCamera();
            osg::StateSet * stateSet = camera->getOrCreateStateSet();
            osg::LightModel* lightmodel = dynamic_cast<osg::LightModel*>(stateSet->getAttribute(osg::StateAttribute::LIGHTMODEL));
            if (lightmodel)
                lightmodel->setAmbientIntensity(osg::Vec4(value, value, value, 1.0f));
        }
    }
    triggerRepaint();
}

void LightSettings::daytimeAmbientBoostChanged(int value)
{
    float real_value = (float)value / 100.0f;
    ui->daytimeAmbientBoostSpinBox->blockSignals(true);
    ui->daytimeAmbientBoostSpinBox->setValue(real_value);
    ui->daytimeAmbientBoostSpinBox->blockSignals(false);
    if(m_sky)
        skySetAmbientDaylightBoost(m_sky, real_value);
    triggerRepaint();
}

void LightSettings::daytimeAmbientBoostChanged(double value)
{
    int int_value = (int)osg::round(value * 100.0);
    ui->daytimeAmbientBoost->blockSignals(true);
    ui->daytimeAmbientBoost->setValue(int_value);
    ui->daytimeAmbientBoost->blockSignals(false);
    if (m_sky)
        skySetAmbientDaylightBoost(m_sky, value);
    triggerRepaint();
}

void LightSettings::exposureChanged(int value)
{
    float real_value = (float)value / 100.0f;
    ui->exposureSpinBox->blockSignals(true);
    ui->exposureSpinBox->setValue(real_value);
    ui->exposureSpinBox->blockSignals(false);
    if (m_sky)
        skySetExposure(m_sky, real_value);
    triggerRepaint();
}

void LightSettings::exposureChanged(double value)
{
    int int_value = (int)osg::round(value * 100.0);
    ui->exposure->blockSignals(true);
    ui->exposure->setValue(int_value);
    ui->exposure->blockSignals(false);
    if (m_sky)
        skySetExposure(m_sky, value);
    triggerRepaint();
}

void LightSettings::globalLightingChanged(bool enable)
{
	ui->lightingMode->setEnabled(enable);
    ui->skyMode->setEnabled(enable);
	ui->diffuseLightColor->setEnabled(enable);
	ui->specularLightColor->setEnabled(enable);
	ui->ambientIntensity->setEnabled(enable);
    ui->ambientBrightness->setEnabled(enable);
    ui->atmosphereVisible->setEnabled(enable);
    ui->sunVisible->setEnabled(enable);
    ui->moonVisible->setEnabled(enable);
    ui->starsVisible->setEnabled(enable);
    ui->skyMode->setEnabled(enable);
    ui->sunPosition->setEnabled(enable);
    ui->moonPosition->setEnabled(enable);
    ui->dateEdit->setEnabled(enable);
    ui->timeEdit->setEnabled(enable);
    apply(false);
}

void LightSettings::timeFactorChanged(double factor)
{
    if(m_sky)
    {
//         m_sky->setTimeFactor(factor, false);
        triggerRepaint();
    }
}

void LightSettings::updateFromSky()
{
//     SceneGraphSky::Mode skymode = SceneGraphSky::ModeInvalid;
//     int index = ui->skyMode->currentIndex();
//     if(index >= 0)
//         skymode = (SceneGraphSky::Mode)ui->skyMode->itemData(index).toInt();


    if(m_sky)
    {
        ui->dateEdit->blockSignals(true);
        ui->timeEdit->blockSignals(true);
        ui->sunPosition->blockSignals(true);
        ui->moonPosition->blockSignals(true);
        QDateTime dateTime;
        QString sunPosition;
        QString moonPosition;

        const osgEarth::DateTime & dt = m_sky->getDateTime();
        dateTime = QDateTime::fromSecsSinceEpoch(dt.asTimeStamp(), Qt::UTC);
//         GpsCoordinate sunPositionGPS = m_sky->getSunPosition();
//         GpsCoordinate moonPositionGPS = m_sky->getMoonPosition();
//         sunPosition = sunPositionGPS.toString().toQString();
//         moonPosition = moonPositionGPS.toString().toQString();

        ui->dateEdit->setDateTime(dateTime);
        ui->timeEdit->setDateTime(dateTime);
        ui->sunPosition->setText(sunPosition);
        ui->moonPosition->setText(moonPosition);

        ui->dateEdit->blockSignals(false);
        ui->timeEdit->blockSignals(false);
        ui->sunPosition->blockSignals(false);
        ui->moonPosition->blockSignals(false);
    }
}

void LightSettings::triggerRepaint()
{
    for (osg::View * view : m_views)
    {
        osgViewer::View* view_ = dynamic_cast<osgViewer::View*>(view);
        if (view_)
            view_->requestRedraw();
    }
}

} // namespace sgi
