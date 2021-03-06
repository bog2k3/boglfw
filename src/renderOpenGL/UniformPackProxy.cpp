#include <boglfw/renderOpenGL/UniformPackProxy.h>
#include <boglfw/renderOpenGL/UniformPack.h>

#include <boglfw/utils/assert.h>

#include <GL/glew.h>

UniformPackProxy::UniformPackProxy(std::shared_ptr<UniformPack> pack)
	: pack_(pack)
{
	assertDbg(pack_ && "invalid uniformPack pointer provided!");
}

UniformPackProxy::UniformPackProxy(UniformPackProxy &&upp)
	: pack_(upp.pack_) {
	upp.pack_.reset();
	uniformIndexes_.swap(upp.uniformIndexes_);
}

// updates the internal mappings between the uniform pack and the opengl program.
void UniformPackProxy::updateMappings(unsigned programId) {
	assertDbg(programId > 0 && "Invalid gl program provided!");
	uniformIndexes_.clear();
	for (unsigned i=0, n=pack_->count(); i<n; i++) {
		auto &desc = pack_->element(i);

		int location = glGetUniformLocation(programId, desc.name.c_str());
		assertDbg(uniformIndexes_.size() == i);
		uniformIndexes_.push_back(location);
	}
}

// pushes all uniform values from the uniform pack into corresponding openGL's uniform locations
void UniformPackProxy::pushValues() {
	for (unsigned i=0; i<uniformIndexes_.size(); i++) {
		if (uniformIndexes_[i] >= 0)
			pack_->pushValue(i, uniformIndexes_[i]);
	}
}
