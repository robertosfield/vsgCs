#pragma once

// Extended DescriptorConfigurator that tracks the set number of the bindings

#include <optional>

#include <vsg/utils/GraphicsPipelineConfigurator.h>

namespace vsgCs {
    class DescriptorSetConfigurator : public vsg::Inherit<vsg::DescriptorConfigurator, class DescriptorSetConfigurator>
    {
      public:
        DescriptorSetConfigurator(uint32_t in_setNumber, vsg::ref_ptr<vsg::ShaderSet> in_shaderSet = {})
            : Inherit(in_shaderSet), setNumber(in_setNumber)
        {}

        DescriptorSetConfigurator(vsg::ref_ptr<vsg::ShaderSet> in_shaderSet = {})
            : Inherit(in_shaderSet)
        {

        }
        bool assignTexture(const std::string& name, vsg::ref_ptr<vsg::Data> textureData = {},
                           vsg::ref_ptr<vsg::Sampler> sampler = {});
        bool assignTexture(const std::string& name, const vsg::ImageInfoList& imageInfoList, uint32_t arrayElement = 0);
        bool assignUniform(const std::string& name, vsg::ref_ptr<vsg::Data> data = {});
        bool assignUniform(const std::string& name, const vsg::BufferInfoList& bufferInfoList, uint32_t arrayElement = 0);
        std::optional<uint32_t> setNumber;
    };
}