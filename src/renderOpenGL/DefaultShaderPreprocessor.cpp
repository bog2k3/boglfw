#include <boglfw/renderOpenGL/DefaultShaderPreprocessor.h>
#include <boglfw/utils/filesystem.h>

#include <sstream>

static bool isWhiteSpace(char c) {
	return c == ' ' || c == '\t';
}

std::string DefaultShaderPreprocessor::preprocess
(std::string const& code, std::string const& originalFilePath) {
	size_t lastWrittenPos = 0;
	size_t linePointer = 0;
	std::stringstream ss;
	const std::string directiveToken = "#include ";
	do {
		// find next directive:
		linePointer = code.find(directiveToken, lastWrittenPos);
		// take out the whitespace before the directive:
		while (linePointer != std::string::npos && linePointer > 0 && isWhiteSpace(code[linePointer-1]))
			linePointer--;
		// write everything up to the directive
		ss << code.substr(lastWrittenPos, linePointer - lastWrittenPos);
		lastWrittenPos = linePointer;

		if (linePointer != std::string::npos && linePointer > 1 && code[linePointer-1] == '/' && code[linePointer-2] == '/') {
			// this #include is commented out, ignore it
			lastWrittenPos = code.find('\n', linePointer);
			continue;
		}

		if (linePointer != std::string::npos) {
			// replace the directive with the contents of the referred file
			size_t directiveEnd = code.find("\n", linePointer);
			if (directiveEnd == std::string::npos)
				directiveEnd = code.length();
			size_t filenameStart = linePointer + directiveToken.length();
			std::string relativePath = code.substr(filenameStart, directiveEnd - filenameStart);
			// this will call preprocessor recursively:
			ss << Shaders::readShaderFile(computePath(relativePath, originalFilePath).c_str());

			lastWrittenPos = directiveEnd;
		}
	} while (lastWrittenPos != std::string::npos);
	return ss.str();
}

std::string DefaultShaderPreprocessor::computePath(std::string const& relativePath, std::string const& fullReferrerPath) {
	return filesystem::getFileDirectory(fullReferrerPath) + relativePath;
}
