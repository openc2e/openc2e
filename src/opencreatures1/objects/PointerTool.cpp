#include "PointerTool.h"

#include "ObjectManager.h"
#include "SFCSerialization.h"
#include "fileformats/sfc/PointerTool.h"

void PointerTool::serialize(SFCContext& ctx, sfc::PointerToolV1* pntr) {
	if (ctx.is_storing()) {
		pntr->relx = relx;
		pntr->rely = rely;
		pntr->bubble = dynamic_cast<sfc::BubbleV1*>(
			ctx.dump_object(g_engine_context.objects->try_get(bubble)).get());
		pntr->text = text;
	} else {
		relx = pntr->relx;
		rely = pntr->rely;
		bubble = ctx.load_object(pntr->bubble);
		text = pntr->text;
		g_engine_context.pointer->m_pointer_tool = uid;
	}
	SimpleObject::serialize(ctx, pntr);
}