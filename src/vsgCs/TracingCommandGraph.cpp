#include "TracingCommandGraph.h"

#include <vsg/io/DatabasePager.h>
#include <vsg/ui/FrameStamp.h>
#include <vsg/vk/State.h>

namespace vsgCs
{
    TracingCollectCommand::TracingCollectCommand(vsg::ref_ptr<TracyContextValue> in_tracyCtx)
        : tracyCtx(in_tracyCtx)
    {
    }

    void TracingCollectCommand::record(vsg::CommandBuffer& cmd) const
    {
#ifdef TRACY_ENABLE
        if (tracyCtx)
        {
            TracyVkCollect(tracyCtx->ctx, cmd.vk());
        }
#endif
    }
    
    TracingCommandGraph::TracingCommandGraph(vsg::ref_ptr<vsg::Window> in_window,
                                             vsg::ref_ptr<vsg::Node> child)
        : Inherit(in_window, child)
    {
        vsg::ref_ptr<TracyContextValue> tracyCtx;
        addChild(TracingCollectCommand::create(tracyCtx));
    }

    void TracingCommandGraph::record(vsg::ref_ptr<vsg::RecordedCommandBuffers> recordedCommandBuffers,
                                     vsg::ref_ptr<vsg::FrameStamp> frameStamp,
                                     vsg::ref_ptr<vsg::DatabasePager> databasePager)
    {
#ifdef TRACY_ENABLE
        if (!tracyCtx)
        {
            if (window && !window->visible())
            {
                return;
            }
            getOrCreateRecordTraversal();
            vsg::ref_ptr<vsg::CommandPool> cp = vsg::CommandPool::create(device, queueFamily,
                                                                    VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
            tmpCmd = cp->allocate(level());
            auto queue = device->getQueue(queueFamily, 0);
            tracyCtx = TracyContextValue::create();
            tracyCtx->ctx = TracyVkContext(device->getPhysicalDevice()->vk(), device->vk(), queue->vk(),
                                           tmpCmd->vk());
            recordTraversal->setObject("tracyCtx", tracyCtx);
            auto collectCmd = children[0].cast<TracingCollectCommand>();
            if (collectCmd)
            {
                collectCmd->tracyCtx = tracyCtx;
            }
        }
#endif
        Inherit::record(recordedCommandBuffers, frameStamp, databasePager);
    }

    void TracingVertexDraw::accept(vsg::RecordTraversal& visitor) const
    {
#ifdef TRACY_ENABLE
        auto tracyCtx = visitor.getObject<TracyContextValue>("tracyCtx");
        TracyVkZone(tracyCtx->ctx, visitor.getState()->_commandBuffer->vk(), "VertexDraw");
#endif
        VertexDraw::accept(visitor);
    }

    void TracingVertexIndexDraw::accept(vsg::RecordTraversal& visitor) const
    {
#ifdef TRACY_ENABLE
        auto tracyCtx = visitor.getObject<TracyContextValue>("tracyCtx");
        TracyVkZone(tracyCtx->ctx, visitor.getState()->_commandBuffer->vk(), "VertexIndexDraw");
#endif
        VertexIndexDraw::accept(visitor);
    }

}
