#ifndef __GJBASEGAMELAYER_H__
#define __GJBASEGAMELAYER_H__

#include <gd.h>
#include <vector>

namespace gd {
	class OBB2D;
	class GJEffectManager;
	class CCNodeContainer;
	class LevelSettingsObject;
	class PlayerObject;
	class GameObject;

	class GJBaseGameLayer : public cocos2d::CCLayer {
	public:
		PAD(4);
		OBB2D* m_boundingBox; // 0x120
		GJEffectManager* m_effectManager; // 0x124
		cocos2d::CCLayer* m_objectLayer; // 0x128
		// these names come from robtop himself, so if you think theyre bad go bother him
		// more explained in depth here https://github.com/Wyliemaster/gddocs/blob/master/docs/resources/client/level-components/capacity-string.md
		cocos2d::CCSpriteBatchNode* m_batchNodeAddTop4; // 0x12C
		cocos2d::CCSpriteBatchNode* m_effectBatchNodeAddTop4; // 0x130
		cocos2d::CCSpriteBatchNode* m_batchNodeTop3; // 0x134
		cocos2d::CCSpriteBatchNode* m_batchNodeAddTop3; // 0x138
		cocos2d::CCSpriteBatchNode* m_batchNodeAddGlowTop3; // 0x13C
		CCNodeContainer* unk140;
		cocos2d::CCSpriteBatchNode* m_batchNodeTextTop3; // 0x144
		cocos2d::CCSpriteBatchNode* m_batchNodeAddTextTop3; // 0x148
		cocos2d::CCSpriteBatchNode* m_effectBatchNodeTop3; // 0x14C
		cocos2d::CCSpriteBatchNode* m_effectBatchNodeAddTop3; // 0x150
		cocos2d::CCSpriteBatchNode* m_batchNodeTop2; // 0x154
		cocos2d::CCSpriteBatchNode* m_batchNodeAddTop2; // 0x158
		cocos2d::CCSpriteBatchNode* m_batchNodeAddGlowTop2; // 0x15C
		CCNodeContainer* unk160;
		cocos2d::CCSpriteBatchNode* m_batchNodeTextTop2; // 0x164
		cocos2d::CCSpriteBatchNode* m_batchNodeAddTextTop2; // 0x168
		cocos2d::CCSpriteBatchNode* m_effectBatchNodeTop2; // 0x16C
		cocos2d::CCSpriteBatchNode* m_effectBatchNodeAddTop2; // 0x170
		cocos2d::CCSpriteBatchNode* m_batchNode; // 0x174
		cocos2d::CCSpriteBatchNode* m_batchNodeAdd; // 0x178
		cocos2d::CCSpriteBatchNode* m_batchNodeAddGlow; // 0x17C
		CCNodeContainer* unk180;
		cocos2d::CCSpriteBatchNode* m_batchNodeTextTop1; // 0x184
		cocos2d::CCSpriteBatchNode* m_batchNodeAddTextTop1; // 0x188
		cocos2d::CCSpriteBatchNode* m_effectBatchNodeTop1; // 0x18C
		cocos2d::CCSpriteBatchNode* m_effectBatchNodeAddTop1; // 0x190
		cocos2d::CCSpriteBatchNode* m_batchNodePlayer; // 0x194
		cocos2d::CCSpriteBatchNode* m_batchNodeAddPlayer; // 0x198
		cocos2d::CCSpriteBatchNode* m_batchNodePlayerGlow; // 0x19C
		cocos2d::CCSpriteBatchNode* m_batchNodeAddMid; // 0x1A0
		cocos2d::CCSpriteBatchNode* m_batchNodeBottom; // 0x1A4
		cocos2d::CCSpriteBatchNode* m_batchNodeAddBottom; // 0x1A8
		cocos2d::CCSpriteBatchNode* m_batchNodeAddBottomGlow; // 0x1AC
		CCNodeContainer* unk1B0;
		cocos2d::CCSpriteBatchNode* m_batchNodeText; // 0x1B4
		cocos2d::CCSpriteBatchNode* m_batchNodeAddText; // 0x1B8
		cocos2d::CCSpriteBatchNode* m_effectBatchNode; // 0x1BC
		cocos2d::CCSpriteBatchNode* m_effectBatchNodeAdd; // 0x1C0
		cocos2d::CCSpriteBatchNode* m_batchNodeBottom2; // 0x1C4
		cocos2d::CCSpriteBatchNode* m_batchNodeAddBottom2; // 0x1C8
		cocos2d::CCSpriteBatchNode* m_batchNodeAddBottom2Glow; // 0x1CC
		CCNodeContainer* unk1D0;
		cocos2d::CCSpriteBatchNode* m_batchNodeTextBot2; // 0x1D4
		cocos2d::CCSpriteBatchNode* m_batchNodeAddTextBot2; // 0x1D8
		cocos2d::CCSpriteBatchNode* m_effectBatchNodeBot2; // 0x1DC
		cocos2d::CCSpriteBatchNode* m_effectBatchNodeAddBot2; // 0x1E0
		cocos2d::CCSpriteBatchNode* m_batchNodeBottom3; // 0x1E4
		cocos2d::CCSpriteBatchNode* m_batchNodeAddBottom3; // 0x1E8
		cocos2d::CCSpriteBatchNode* m_batchNodeAddBottom3Glow; // 0x1EC
		CCNodeContainer* unk1F0;
		cocos2d::CCSpriteBatchNode* m_batchNodeTextBot3; // 0x1F4
		cocos2d::CCSpriteBatchNode* m_batchNodeAddTextBot3; // 0x1F8
		cocos2d::CCSpriteBatchNode* m_effectBatchNodeBot3; // 0x1FC
		cocos2d::CCSpriteBatchNode* m_effectBatchNodeAddBot3; // 0x200
		cocos2d::CCSpriteBatchNode* m_batchNodeBottom4; // 0x204
		cocos2d::CCSpriteBatchNode* m_batchNodeAddBottom4; // 0x208
		cocos2d::CCSpriteBatchNode* m_batchNodeAddBottom4Glow; // 0x20C
		CCNodeContainer* unk210;
		cocos2d::CCSpriteBatchNode* m_batchNodeTextBot4; // 0x214
		cocos2d::CCSpriteBatchNode* m_batchNodeAddTextBot4; // 0x218
		cocos2d::CCSpriteBatchNode* m_effectBatchNodeBot4; // 0x21C
		cocos2d::CCSpriteBatchNode* m_effectBatchNodeAddBot4; // 0x220
		PlayerObject* m_player1; // 0x224
		PlayerObject* m_player2; // 0x228
		LevelSettingsObject* m_levelSettings; // 0x22C
		cocos2d::CCDictionary* m_disabledGroupsDict; // 0x230 ?
		cocos2d::CCArray* m_objects; // 0x234
		cocos2d::CCArray* m_sectionObjects; // 0x238
		cocos2d::CCArray* m_sections; // 0x23C
		cocos2d::CCArray* m_collisionBlocks; // 0x240
		cocos2d::CCArray* m_spawnObjects; // 0x244
		cocos2d::CCArray* unk248;
		cocos2d::CCNode* m_groupNodes; // 0x24C
		std::vector<GameObject*> m_objectsVec; // 0x250 great name i know
		std::vector<GameObject*> m_disabledObjects; // 0x25C
		cocos2d::CCDictionary* m_groupDict; // 0x268
		cocos2d::CCDictionary* m_staticGroupDict; // 0x26C
		cocos2d::CCDictionary* m_optimizedGroupDict; // 0x270
		std::vector<cocos2d::CCArray*> m_groups; // 0x274
		std::vector<cocos2d::CCArray*> m_staticGroups; // 0x280
		std::vector<cocos2d::CCArray*> m_optimizedGroups; // 0x28C
		cocos2d::CCArray* m_batchNodes; // 0x298
		cocos2d::CCArray* m_processedGroups; // 0x29C
		cocos2d::CCDictionary* m_counterDict; // 0x2A0
		cocos2d::CCDictionary* m_spawnedGroups; // 0x2A4
		bool m_didUpdateNormalCapacity; // 0x2A8
		bool m_isDualMode; // 0x2A9
		int unk2AC;
		bool m_activeDualTouch; // 0x2B0
		int m_attemptClickCount; // 0x2B4
		int m_currentSection; // 0x2B8
		int m_oldSection; // 0x2BC
		bool m_hasDisabledObjects; // 0x2C0
		bool m_blending; // 0x2C1
		PAD(10);
		
		void pushButton(int unk, bool player1) {
			return reinterpret_cast<void(__thiscall*)(GJBaseGameLayer*, int, bool)>(base + 0x111500)(this, unk, player1);
		}

		void releaseButton(int unk, bool player1) {
			return reinterpret_cast<void(__thiscall*)(GJBaseGameLayer*, int, bool)>(base + 0x111660)(this, unk, player1);
		}
	};
}

#endif
