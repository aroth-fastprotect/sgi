#pragma once

#include <sgi/plugins/SGISettingsDialogImpl>
#include <set>

QT_BEGIN_NAMESPACE
class Ui_LightSettings;
class QLineEdit;
QT_END_NAMESPACE

namespace osg {
    class View;
    class Light;
    class Camera;
}

namespace osgEarth {
    namespace Util {
        class SkyNode;
    }
}

namespace sgi {

class LightSettings : public SettingsQDialogImpl
{
	Q_OBJECT

public:
    LightSettings(QWidget * parent, SGIPluginHostInterface * hostInterface, SGIItemBase * item, ISettingsDialogInfo * info);
    ~LightSettings() override;

    typedef std::set<osg::View*> ViewSet;

public slots:
	void				save();
	void				apply();
	virtual void		reject();
	void				restoreDefaults();
	void				load();
    void                reset();

	void				changeDiffuseLightColor();
	void				changeSpecularLightColor();

    void				skyModeChanged(int index);
	void				globalLightingChanged(bool enable);

    void                setAtmosphericLighting(bool enable);
    void				setAtmosphereVisible(bool visible);
    void				setSunVisible(bool visible);
    void				setMoonVisible(bool visible);
    void				setStarsVisible(bool visible);

    void				setSunPosition(const QString & position);
    void				setMoonPosition(const QString & position);
    void				setDateTime(const QDateTime & datetime);

    void                lightingModeChanged(int);
    void                ambientBrightnessChanged(int);
    void                ambientBrightnessChanged(double);
    void                ambientIntensityChanged(int);
    void                ambientIntensityChanged(double);
    void                daytimeAmbientBoostChanged(int);
    void                daytimeAmbientBoostChanged(double);
    void                exposureChanged(int);
    void                exposureChanged(double);

    void                timeFactorChanged(double);

    void                updateFromSky();

protected:
	void				apply(bool save);

	virtual void 		showEvent ( QShowEvent * event );

private:
    void                initialize();
    void                triggerRepaint();
    void                enableSkyModeParameterControls();
    static void         setColorToLineEdit(QLineEdit * edit, const QColor & color);

private:
	Ui_LightSettings *				ui;
    osgEarth::Util::SkyNode *       m_sky;
    osg::Light *                    m_light;
    osg::Camera *                   m_camera;
    ViewSet                         m_views;
	bool							m_ready;
    QColor							m_ambientLightColor;
	QColor							m_diffuseLightColor;
	QColor							m_specularLightColor;
    QTimer *                        m_updateTimer;
};

} // namespace sgi
