// kate: syntax C++11;
// SGI - Copyright (C) 2012-2018 FAST Protect, Andreas Roth

// tricking the SGI_IMPL_EXPORT defined to switch to export
#define SGI_IMPL_LIBRARY
#include <sgi/plugins/SGIShaderLoader>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>

namespace sgi {

bool ShaderPackage::load(
    QOpenGLShaderProgram * program,
    ShaderType type,
    const std::string&    filename)
{
    std::string source = ShaderLoader::load(filename, *this);
    QOpenGLShader::ShaderType qtype;
    switch(type)
    {
        case ShaderTypeVertex: qtype = QOpenGLShader::Vertex; break;
        case ShaderTypeFragment: qtype = QOpenGLShader::Fragment; break;
        case ShaderTypeGeometry: qtype = QOpenGLShader::Geometry; break;
    }
    return program->addShaderFromSourceCode(qtype, QByteArray::fromStdString(source));
}

} // namespace sgi
