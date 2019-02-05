#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class Ui_ShaderFunctionDetails;
QT_END_NAMESPACE

namespace osg {
    template<class T>
    class ref_ptr;
}

namespace osgEarth {
    class VirtualProgram;
}

namespace sgi {

class ShaderFunctionDetailsPrivate;

class ShaderFunctionDetails : public QDialog
{
    Q_DECLARE_PRIVATE(ShaderFunctionDetails);
    Q_OBJECT

public:
    ShaderFunctionDetails(osgEarth::VirtualProgram * vp=nullptr, unsigned functionHash=0, QWidget * parent=nullptr);
    ~ShaderFunctionDetails() override;

    bool hasModifications() const;

protected slots:
    void reset();
    void load();

    void orderChanged(double v);
    void locationChanged(int index);

private:
    bool applyChanges(int location, float order);

private:  // for now
    Ui_ShaderFunctionDetails* ui;
    ShaderFunctionDetailsPrivate * d_ptr;
};

} // namespace sgi
