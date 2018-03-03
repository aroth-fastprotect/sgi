// kate: syntax C++11;
// SGI - Copyright (C) 2012-2018 FAST Protect, Andreas Roth

#include <sgi/plugins/SGIShaderLoader>
#include <sgi/helpers/string>
#include <algorithm>

namespace sgi {



// find the value of a pragma, e.g.:
//   #pragma oe_key value
// returns the string "value" (without the quotes).
std::string
ShaderLoader::getPragmaValue(const std::string& source, const std::string& key)
{
    std::string token("#pragma " + key);
    std::string::size_type statementPos = source.find(token);
    if ( statementPos == std::string::npos )
        return "";

    // no quotes; parse to newline.
    std::string::size_type startPos = source.find_first_not_of(" \t", statementPos+token.length());
    if ( startPos == std::string::npos )
        return ""; // no whitespace after the pragma key

    std::string::size_type newlinePos = source.find('\n', startPos);
    if ( newlinePos == std::string::npos )
        return ""; // new newline found after pragma

    return helpers::trim(source.substr(startPos, newlinePos-startPos));
}

void
ShaderLoader::getAllPragmaValues(const std::string&     source,
                                 const std::string&     key,
                                 std::set<std::string>& output)
{
    std::string token("#pragma " + key);
    std::string::size_type pragmaPos = 0;
    while( pragmaPos != std::string::npos )
    {
        pragmaPos = source.find(token, pragmaPos);
        if ( pragmaPos != std::string::npos )
        {
            std::string::size_type startPos = source.find_first_not_of(" \t", pragmaPos+token.length());
            if ( startPos != std::string::npos )
            {
                std::string::size_type newlinePos = source.find('\n', startPos);
                if ( newlinePos != std::string::npos )
                {
                    const size_t len = newlinePos - startPos;
                    if ( len > 0 )
                    {
                        output.insert( helpers::trim(source.substr(startPos, len)) );
                    }
                    pragmaPos = newlinePos;
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
}

std::string
ShaderLoader::load(const std::string&    filename,
                   const ShaderPackage&  package)
{
    std::string output;
    bool useInlineSource = false;

    std::string inlineSource;
    ShaderPackage::SourceMap::const_iterator source = package._sources.find(filename);
    if ( source != package._sources.end() )
        inlineSource = source->second;

    output = inlineSource;
    useInlineSource = true;

    // replace common tokens:
    helpers::replaceIn(output, "$GLSL_VERSION_STR", GLSL_VERSION_STR);
    helpers::replaceIn(output, "$GLSL_DEFAULT_PRECISION_FLOAT", GLSL_DEFAULT_PRECISION_FLOAT);

    // If we're using inline source, we have to post-process the string.
    if ( useInlineSource )
    {
        // Replace tokens inserted in the CMakeModules/ConfigureShaders.cmake.in script.
        helpers::replaceIn(output, "%EOL%",   "\n");
        helpers::replaceIn(output, "%QUOTE%", "\"");
    }

    // Process any "#pragma include" statements
    while(true)
    {
        const std::string token("#pragma include");
        std::string::size_type statementPos = output.find(token);
        if ( statementPos == std::string::npos )
            break;

        std::string::size_type startPos = output.find_first_not_of(" \t", statementPos+token.length());
        if ( startPos == std::string::npos )
            break;

        std::string::size_type endPos = output.find('\n', startPos);
        if ( endPos == std::string::npos )
            break;

        std::string statement( output.substr(statementPos, endPos-statementPos) );
        std::string fileToInclude( helpers::trim(output.substr(startPos, endPos-startPos)) );

        // load the source of the included file, and append a newline so we
        // don't break the MULTILINE macro if the last line of the include
        // file is a comment.
        std::string fileSource = helpers::Stringify()
            << load(fileToInclude, package)
            << "\n";

        helpers::replaceIn(output, statement, fileSource);
    }

    // Process any "#pragma define" statements
    while (true)
    {
        const std::string token("#pragma vp_define");
        std::string::size_type statementPos = output.find(token);
        if (statementPos == std::string::npos)
            break;

        std::string::size_type startPos = output.find_first_not_of(" \t", statementPos + token.length());
        if (startPos == std::string::npos)
            break;

        std::string::size_type endPos = output.find('\n', startPos);
        if (endPos == std::string::npos)
            break;

        std::string statement(output.substr(statementPos, endPos - statementPos));
        std::string varName(helpers::trim(output.substr(startPos, endPos - startPos)));

        ShaderPackage::DefineMap::const_iterator d = package._defines.find(varName);

        bool defineIt =
            d != package._defines.end() &&
            d->second == true;

        std::string newStatement = helpers::Stringify()
            << (defineIt ? "#define " : "#undef ")
            << varName;

        helpers::replaceIn(output, statement, newStatement);
    }

    // Process any replacements.
    for (ShaderPackage::ReplaceMap::const_iterator i = package._replaces.begin();
        i != package._replaces.end();
        ++i)
    {
        helpers::replaceIn(output, i->first, i->second);
    }

    // Lastly, remove any CRs
    helpers::replaceIn(output, "\r", "");

    return output;
}

std::string
ShaderLoader::load(const std::string&    filename,
                   const std::string&    inlineSource)
{
    std::string output;
    bool useInlineSource = false;

    output = inlineSource;
    useInlineSource = true;

    // replace common tokens:
    helpers::replaceIn(output, "$GLSL_VERSION_STR", GLSL_VERSION_STR);
    helpers::replaceIn(output, "$GLSL_DEFAULT_PRECISION_FLOAT", GLSL_DEFAULT_PRECISION_FLOAT);

    // If we're using inline source, we have to post-process the string.
    if (useInlineSource)
    {
        // Replace tokens inserted in the CMakeModules/ConfigureShaders.cmake.in script.
        helpers::replaceIn(output, "%EOL%", "\n");
        helpers::replaceIn(output, "%QUOTE%", "\"");
    }

    // Lastly, remove any CRs
    helpers::replaceIn(output, "\r", "");

    return output;
}

void
ShaderLoader::split(const std::string& multisource,
                    std::vector<std::string>& output)
{
#define SPLIT_DELIM "[break]"
#define SPLIT_DELIM_LEN 7
    std::string::size_type offset = 0, pos = 0;
    while ((pos = multisource.find(SPLIT_DELIM, offset)) != std::string::npos)
    {
        std::string source = multisource.substr(offset, pos-offset);
        output.push_back(source);
        offset = pos + SPLIT_DELIM_LEN;
    }
    output.push_back(multisource.substr(offset));
}

//...................................................................

void
ShaderPackage::define(const std::string& name,
                      bool               defOrUndef)
{
    _defines[name] = defOrUndef;
}

void
ShaderPackage::replace(const std::string& pattern,
                       const std::string& value)
{
    _replaces[pattern] = value;
}

} // namespace sgi
