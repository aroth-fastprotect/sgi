#include "stdafx.h"
#include <sgi/plugins/Matrix>
#include <sgi/plugins/Quat>
#include <sgi/plugins/Vector>
#include "MatrixInputDialog.h"

#include <unordered_map>
#include <QPushButton>
#include <QTextStream>

#include <ui_MatrixInputDialog.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

    QTextStream & operator<< (QTextStream & ts, const Vector3 & v)
    {
        ts << v.x() << ',' << v.y() << ',' << v.z();
        return ts;
    }
    QTextStream & operator<< (QTextStream & ts, const Vector4 & v)
    {
        ts << v.x() << ',' << v.y() << ',' << v.z() << ',' << v.w();
        return ts;
    }
    QTextStream & operator<< (QTextStream & ts, const Quat & q)
    {
        ts << q.x() << ',' << q.y() << ',' << q.z() << ',' << q.w();
        return ts;
    }

void MatrixInputDialog::formatMatrixValue(const Matrix & matrix, QString & text, MatrixUsage usage)
{
    text.clear();
    QTextStream ts(&text);
    switch(usage)
    {
    case MatrixUsageUnknown:
    default:
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col)
                ts << matrix(row, col) << " ";
            ts << endl;
        }
        break;
    case MatrixUsageProjectionPerspective:
        {
#if 0
            osg::Matrixd pm(matrix.ptr());
            double fovy, aspectRatio, zNear, zFar;
            pm.getPerspective(fovy, aspectRatio, zNear, zFar);
            ts << "fovy: " << fovy << endl;
            ts << "aspectRatio: " << aspectRatio << endl;
            ts << "zNear: " << zNear << endl;
            ts << "zFar: " << zFar << endl;
#endif
        }
        break;
    case MatrixUsageProjectionOrtho:
        {
#if 0
            osg::Matrixd pm(matrix.ptr());
            double left, right, bottom, top, zNear, zFar;
            pm.getOrtho(left, right, bottom, top, zNear, zFar);
            ts << "left: " << left << endl;
            ts << "right: " << right << endl;
            ts << "bottom: " << bottom << endl;
            ts << "top: " << top << endl;
            ts << "zNear: " << zNear << endl;
            ts << "zFar: " << zFar << endl;
#endif
        }
        break;
    case MatrixUsageProjectionOrtho2D:
        {
#if 0
            osg::Matrixd pm(matrix.ptr());
            double left, right, bottom, top, zNear, zFar;
            pm.getOrtho(left, right, bottom, top, zNear, zFar);
            ts << "left: " << left << endl;
            ts << "right: " << right << endl;
            ts << "bottom: " << bottom << endl;
            ts << "top: " << top << endl;
#endif
        }
        break;
    case MatrixUsageProjectionFrustum:
        {
#if 0
            osg::Matrixd pm(matrix.ptr());
            double left, right, bottom, top, zNear, zFar;
            pm.getFrustum(left, right, bottom, top, zNear, zFar);
            ts << "left: " << left << endl;
            ts << "right: " << right << endl;
            ts << "bottom: " << bottom << endl;
            ts << "top: " << top << endl;
            ts << "zNear: " << zNear << endl;
            ts << "zFar: " << zFar << endl;
#endif
        }
        break;
    case MatrixUsageView:
        {
#if 0
            osg::Matrixd pm(matrix.ptr());
            osg::Vec3d eye, center, up;
            pm.getLookAt(eye, center, up);
            ts << "eye: " << eye << endl;
            ts << "center: " << center << endl;
            ts << "up: " << up << endl;
#endif
        }
        break;
    case MatrixUsageModel:
        {
#if 0
            osg::Matrixd pm(matrix.ptr());
            osg::Vec3d translate, scale;
            osg::Quat rotation, so;
            pm.decompose(translate, rotation, scale, so);
            ts << "translate: " << translate << endl;
            ts << "rotation: " << rotation << endl;
            ts << "scale: " << scale << endl;
            ts << "so: " << so << endl;
#endif
        }
        break;
    }
    ts.flush();
}

typedef QMap<QString, QString> QStringMap;

namespace {
    double readDouble(const QStringMap & map, const QString & key, bool & ok)
    {
        double ret = 0;
        if (ok)
        {
            auto it = map.find(key);
            if (it != map.end())
            {
                const QString & value = it.value();
                ret = value.toDouble(&ok);
            }
        }
        return ret;
    }
    Vector3 readVec3d(const QStringMap & map, const QString & key, bool & ok)
    {
        Vector3 ret;
        if (ok)
        {
            auto it = map.find(key);
            if (it != map.end())
            {
                const QString & value = it.value();
                QStringList elems = value.split(',', QString::SkipEmptyParts);
                if (elems.size() == 3)
                {
                    bool x_ok = false, y_ok = false, z_ok = false;
                    double x = elems[0].toDouble(&x_ok);
                    double y = elems[1].toDouble(&y_ok);
                    double z = elems[2].toDouble(&z_ok);
                    ok = (x_ok && y_ok && z_ok);
                    if (ok)
                        ret.set(x, y, z);
                }
            }
        }
        return ret;
    }
    Quat readQuat(const QStringMap & map, const QString & key, bool & ok)
    {
        Quat ret;
        if (ok)
        {
            auto it = map.find(key);
            if (it != map.end())
            {
                const QString & value = it.value();
                QStringList elems = value.split(',', QString::SkipEmptyParts);
                if (elems.size() == 4)
                {
                    bool x_ok = false, y_ok = false, z_ok = false, w_ok = false;
                    double x = elems[0].toDouble(&x_ok);
                    double y = elems[1].toDouble(&y_ok);
                    double z = elems[2].toDouble(&z_ok);
                    double w = elems[3].toDouble(&w_ok);
                    ok = (x_ok && y_ok && z_ok && w_ok);
                    if (ok)
                        ret.set(x, y, z, w);
                }
            }
        }
        return ret;
    }
}

bool MatrixInputDialog::parseMatrixValue(Matrix & matrix, const QString & text, MatrixUsage usage, bool * ok)
{
    bool ret = false;
    QStringList lines = text.split(QChar('\n'));
    QStringMap kvmap;
    for (const QString & line : lines)
    {
        int index = line.indexOf(QChar(':'));
        if (index > 0)
        {
            QString key = line.mid(0, index).trimmed();
            QString value = line.mid(index+1).trimmed();
            if(!value.isEmpty())
                kvmap[key] = value;
        }
    }
    switch(usage)
    {
    case MatrixUsageUnknown:
    default:
        ret = lines.size() == 4;
        for(unsigned row = 0; ret && row < (unsigned)lines.size(); ++row)
        {
            const QString & line = lines[row];
            QStringList elems = line.split(' ');
            ret = elems.size() == 4;
            for(unsigned col = 0; ret && col < (unsigned)elems.size(); ++col)
            {
                const QString & elem = elems[col];
                bool v_ok = false;
                double v = elem.toDouble(&v_ok);
                if(v_ok)
                    matrix(row, col) = v;
                else
                    ret = false;
            }
        }
        break;
    case MatrixUsageProjectionPerspective:
        {
            bool v_ok = true;
            double fovy = readDouble(kvmap, "fovy", v_ok);
            double aspectRatio = readDouble(kvmap, "aspectratio", v_ok);
            double zNear = readDouble(kvmap, "znear", v_ok);
            double zFar = readDouble(kvmap, "zfar", v_ok);
            if (v_ok)
            {
#if 0
                osg::Matrixd m;
                m.makePerspective(fovy, aspectRatio, zNear, zFar);
                matrix.set(m.ptr());
                ret = true;
#endif
            }
        }
        break;
    case MatrixUsageProjectionOrtho:
        {
            bool v_ok = true;
            double left = readDouble(kvmap, "left", v_ok);
            double right = readDouble(kvmap, "right", v_ok);
            double bottom = readDouble(kvmap, "bottom", v_ok);
            double top = readDouble(kvmap, "top", v_ok);
            double zNear = readDouble(kvmap, "znear", v_ok);
            double zFar = readDouble(kvmap, "zfar", v_ok);
            if (v_ok)
            {
#if 0
                osg::Matrixd m;
                m.makeOrtho(left, right, bottom, top, zNear, zFar);
                matrix.set(m.ptr());
                ret = true;
#endif
            }
        }
        break;
    case MatrixUsageProjectionOrtho2D:
        {
            bool v_ok = true;
            double left = readDouble(kvmap, "left", v_ok);
            double right = readDouble(kvmap, "right", v_ok);
            double bottom = readDouble(kvmap, "bottom", v_ok);
            double top = readDouble(kvmap, "top", v_ok);
            if (v_ok)
            {
#if 0
                osg::Matrixd m;
                m.makeOrtho2D(left, right, bottom, top);
                matrix.set(m.ptr());
                ret = true;
#endif
            }
        }
        break;
    case MatrixUsageProjectionFrustum:
        {
            bool v_ok = true;
            double left = readDouble(kvmap, "left", v_ok);
            double right = readDouble(kvmap, "right", v_ok);
            double bottom = readDouble(kvmap, "bottom", v_ok);
            double top = readDouble(kvmap, "top", v_ok);
            double zNear = readDouble(kvmap, "znear", v_ok);
            double zFar = readDouble(kvmap, "zfar", v_ok);
            if (v_ok)
            {
#if 0
                osg::Matrixd m;
                m.makeFrustum(left, right, bottom, top, zNear, zFar);
                matrix.set(m.ptr());
                ret = true;
#endif
            }
        }
        break;
    case MatrixUsageView:
        {
            bool v_ok = true;
            Vector3 eye = readVec3d(kvmap, "eye", v_ok);
            Vector3 center = readVec3d(kvmap, "center", v_ok);
            Vector3 up = readVec3d(kvmap, "up", v_ok);
            if (v_ok)
            {
#if 0
                osg::Matrixd m;
                m.makeLookAt(eye, center, up);
                matrix.set(m.ptr());
                ret = true;
#endif
            }
        }
        break;
    case MatrixUsageModel:
        {
            bool v_ok = true;
            Vector3 translate = readVec3d(kvmap, "translate", v_ok);
            Vector3 scale = readVec3d(kvmap, "scale", v_ok);
            Quat so = readQuat(kvmap, "so", v_ok);
            Quat rotation = readQuat(kvmap, "rotation", v_ok);
            if (v_ok)
            {
#if 0
                osg::Matrixd m = osg::Matrixd::identity();
                m.postMultTranslate(translate);
                m.postMultTranslate(scale);
                m.postMultRotate(rotation);
                m.postMultRotate(so);
                matrix.set(m.ptr());
                ret = true;
#endif
            }
        }
        break;

    }
    if(ok)
        *ok = ret;
    return ret;
}

MatrixInputDialog::MatrixInputDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , ui(nullptr)
    , _original_value()
    , _original_usage(MatrixUsageUnknown)
    , _value()
    , _usage(MatrixUsageUnknown)
{
    ui = new Ui_MatrixInputDialog;
    ui->setupUi( this );

    ui->tabWidget->setCurrentWidget(ui->tabUser);
    ui->representationComboBoxRaw->setEnabled(false);
    fillMatrixUsageType(ui->representationComboBoxRaw, MatrixUsageUnknown);
    fillMatrixUsageType(ui->representationComboBoxUser, MatrixUsageUnknown);

    connect(ui->buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reset()));
}

MatrixInputDialog::~MatrixInputDialog()
{
    if (ui)
    {
        delete ui;
        ui = nullptr;
    }
}

void MatrixInputDialog::fillMatrixUsageType(QComboBox * comboBox, MatrixUsage usage)
{
    comboBox->clear();
    comboBox->addItem(tr("Raw"), MatrixUsageUnknown);
    comboBox->addItem(tr("Model"), MatrixUsageModel);
    comboBox->addItem(tr("View"), MatrixUsageView);
    comboBox->addItem(tr("Perspective projection"), MatrixUsageProjectionPerspective);
    comboBox->addItem(tr("Orthographic projection"), MatrixUsageProjectionOrtho);
    comboBox->addItem(tr("Orthographic 2D projection"), MatrixUsageProjectionOrtho2D);
    comboBox->addItem(tr("Projection frustum"), MatrixUsageProjectionFrustum);
    int index = comboBox->findData(QVariant((int)usage));
    comboBox->setCurrentIndex(index);
}

void MatrixInputDialog::parseMatrixValue(const QString & text, MatrixUsage usage)
{
    Matrix tmp = _value;
    if(parseMatrixValue(tmp, text, usage))
        _value = tmp;
}

void MatrixInputDialog::parseMatrixValue(QPlainTextEdit * textEdit, MatrixUsage usage)
{
    parseMatrixValue(textEdit->toPlainText(), usage);
}

void MatrixInputDialog::formatMatrixValue(QString & text, MatrixUsage usage)
{
    formatMatrixValue(_value, text, usage);
}

void MatrixInputDialog::formatMatrixValue(QPlainTextEdit * textEdit, MatrixUsage usage)
{
    QString text;
    formatMatrixValue(text, usage);
    textEdit->blockSignals(true);
    textEdit->setPlainText(text);
    textEdit->blockSignals(false);
}


void MatrixInputDialog::setLabel(const QString & message)
{
    ui->label->setText(message);
}

QString MatrixInputDialog::label() const
{
    return ui->label->text();
}

void MatrixInputDialog::reset()
{
    setValue(_original_value, _usage);
}

void MatrixInputDialog::setOriginalValue(const Matrix & value, MatrixUsage usage)
{
    _original_value = value;
    _original_usage = usage;
}

const Matrix & MatrixInputDialog::originalValue() const
{
    return _original_value;
}

void MatrixInputDialog::setValue(const Matrix & value, MatrixUsage usage)
{
    _value = value;
    _usage = usage;

    int index = ui->representationComboBoxUser->findData(QVariant((int)usage));
    ui->representationComboBoxUser->blockSignals(true);
    ui->representationComboBoxUser->setCurrentIndex(index);
    ui->representationComboBoxUser->blockSignals(false);

    MatrixUsage rawUsage = (MatrixUsage)ui->representationComboBoxRaw->itemData(ui->representationComboBoxRaw->currentIndex()).toInt();
    formatMatrixValue(ui->textEditRaw, rawUsage);
    formatMatrixValue(ui->textEditUser, usage);
}

const Matrix & MatrixInputDialog::value() const
{
    return _value;
}

void MatrixInputDialog::setDecimals(int decimals)
{
}

int MatrixInputDialog::decimals() const
{
    return 0;
}

void MatrixInputDialog::rawRepresentationChanged(int index)
{
    MatrixUsage usage = (MatrixUsage)ui->representationComboBoxRaw->itemData(index).toInt();
    formatMatrixValue(ui->textEditRaw, usage);
}

void MatrixInputDialog::userRepresentationChanged(int index)
{
    MatrixUsage usage = (MatrixUsage)ui->representationComboBoxUser->itemData(index).toInt();
    formatMatrixValue(ui->textEditUser, usage);
}

void MatrixInputDialog::rawTextEditChanged()
{
    int index = ui->representationComboBoxRaw->currentIndex();
    MatrixUsage usage = (MatrixUsage)ui->representationComboBoxRaw->itemData(index).toInt();
    parseMatrixValue(ui->textEditRaw, usage);

    index = ui->representationComboBoxUser->currentIndex();
    usage = (MatrixUsage)ui->representationComboBoxUser->itemData(index).toInt();
    formatMatrixValue(ui->textEditUser, usage);
}

void MatrixInputDialog::userTextEditChanged()
{
    int index = ui->representationComboBoxUser->currentIndex();
    MatrixUsage usage = (MatrixUsage)ui->representationComboBoxUser->itemData(index).toInt();
    parseMatrixValue(ui->textEditUser, usage);

    index = ui->representationComboBoxRaw->currentIndex();
    usage = (MatrixUsage)ui->representationComboBoxRaw->itemData(index).toInt();
    formatMatrixValue(ui->textEditRaw, usage);
}

} // namespace sgi
