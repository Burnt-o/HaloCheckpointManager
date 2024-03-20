#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"
// Very similiar to GUIHeading but with less visual contrast and a parameter to add a left margin to it and it's child elements
template<bool startsOpen>
class GUISkullToggle : public IGUIElement {

private:
	std::string mHeadingText;
	bool headingOpen = false;
	float mLeftMargin;
	std::weak_ptr<SettingsStateAndEvents> mSettingsWeak;


	struct skullRenderInfo 
	{
		std::string displayName;
		std::string toolTip;
		RebindableHotkeyEnum hotkeyEnum;
	};

	std::map<SkullEnum, skullRenderInfo> mSkullRenderInfo =
	{
		{ SkullEnum::Anger,{ "Anger", "Enemies fire their weapons faster and more frequently.", RebindableHotkeyEnum::skullAngerHotkey}},
		{ SkullEnum::Assassins,{ "Assassins", "All enemies in the game are permanently cloaked.", RebindableHotkeyEnum::skullAssassinsHotkey}},
		{ SkullEnum::BlackEye,{ "Black Eye", "Your shields only recharge when you melee enemies. Bash your way to better health!", RebindableHotkeyEnum::skullBlackEyeHotkey}},
		{ SkullEnum::Blind,{ "Blind", "HUD and weapon do not display onscreen.", RebindableHotkeyEnum::skullBlindHotkey}},
		{ SkullEnum::Catch,{ "Catch", "Enemies throw and drop more grenades.", RebindableHotkeyEnum::skullCatchHotkey}},
		{ SkullEnum::EyePatch,{ "Eye Patch", "Auto aim features disabled for all weapons. You'll miss it when it's gone.", RebindableHotkeyEnum::skullEyePatchHotkey}},
		{ SkullEnum::Famine,{ "Famine", "Weapons dropped by AI have half the ammo they normally would. You might want to pack an extra magazine.", RebindableHotkeyEnum::skullFamineHotkey}},
		{ SkullEnum::Fog,{ "Fog", "The motion tracker is disabled. You'll miss those eyes in the back of your head.", RebindableHotkeyEnum::skullFogHotkey}},
		{ SkullEnum::Foreign,{ "Foreign", "Alien weapons: How do they work? Players cannot pick up or use opposing factions' weapons.", RebindableHotkeyEnum::skullForeignHotkey}},
		{ SkullEnum::Iron,{ "Iron", "Death carries a heavy price. Dying in co-op resets you to your last saved checkpoint. Dying solo restarts the level.", RebindableHotkeyEnum::skullIronHotkey}},
		{ SkullEnum::Jacked,{ "Jacked", "Players can only enter ground vehicles by boarding.", RebindableHotkeyEnum::skullJackedHotkey}},
		{ SkullEnum::Masterblaster,{ "Masterblaster", "Co-op skull: One player's shields recharge to overshield level and can only melee while the second player has no shields but infinite ammo. Roles switch after a number of enemy kills.", RebindableHotkeyEnum::skullMasterblasterHotkey}},
		{ SkullEnum::Mythic,{ "Mythic", "Enemies have increased health.", RebindableHotkeyEnum::skullMythicHotkey}},
		{ SkullEnum::Recession,{ "Recession", "Every shot is worth twice the ammo. Save those bullets, player.", RebindableHotkeyEnum::skullRecessionHotkey}},
		{ SkullEnum::SoAngry,{ "So Angry", "Enraged Brutes explode like a grenade if not dispatched within seconds. Base Brute score values are increased.", RebindableHotkeyEnum::skullSoAngryHotkey}},
		{ SkullEnum::Streaking,{ "Streaking", "Player shields constantly decay at a slow pace but recharge some every enemy kill.", RebindableHotkeyEnum::skullStreakingHotkey}},
		{ SkullEnum::Swarm,{ "Swarm", "Hunters are much stronger. Base Hunter score values are increased.", RebindableHotkeyEnum::skullSwarmHotkey}},
		{ SkullEnum::ThatsJustWrong,{ "That's Just Wrong", "Increases enemy awareness of player.", RebindableHotkeyEnum::skullThatsJustWrongHotkey}},
		{ SkullEnum::TheyComeBack,{ "They Come Back", "Flood Combat Forms spawned by Infection Forms reanimating a corpse are much more dangerous.", RebindableHotkeyEnum::skullTheyComeBackHotkey}},
		{ SkullEnum::Thunderstorm,{ "Thunderstorm", "Upgrades the rank of most enemies.", RebindableHotkeyEnum::skullThunderstormHotkey}},
		{ SkullEnum::Tilt,{ "Tilt", "Enemy resistances and weaknesses are increased.", RebindableHotkeyEnum::skullTiltHotkey}},
		{ SkullEnum::ToughLuck,{ "Tough Luck", "Enemies always go berserk, always dive out of the way, never flee.", RebindableHotkeyEnum::skullToughLuckHotkey}},
		{ SkullEnum::Bandanna,{ "Bandanna", "Infinite Ammo, Grenades, Armor Ability, and more…", RebindableHotkeyEnum::skullBandannaHotkey}},
		{ SkullEnum::BondedPair,{ "Bonded Pair", "Co-op skull: When one player dies, the other player gets a large damage boost that lasts a while", RebindableHotkeyEnum::skullBondedPairHotkey}},
		{ SkullEnum::Boom,{ "Boom", "Once the skull is activated, explosion radius will increase x2.", RebindableHotkeyEnum::skullBoomHotkey}},
		{ SkullEnum::Cowbell,{ "Cowbell", "Acceleration from explosions is increased.", RebindableHotkeyEnum::skullCowbellHotkey}},
		{ SkullEnum::Envy,{ "Envy", "Chief trades his flashlight for Active Camo.", RebindableHotkeyEnum::skullEnvyHotkey}},
		{ SkullEnum::Feather,{ "Feather", "Player and enemy melee strikes impart more movement physics.", RebindableHotkeyEnum::skullFeatherHotkey}},
		{ SkullEnum::Ghost,{ "Ghost", "AI no longer flinch from attacks.", RebindableHotkeyEnum::skullGhostHotkey}},
		{ SkullEnum::GruntBirthdayParty,{ "Grunt Birthday", "Grunt headshots lead to glorious celebrations.", RebindableHotkeyEnum::skullGruntBirthdayPartyHotkey}},
		{ SkullEnum::GruntFuneral,{ "Grunt Funeral", "Grunts die in a blaze of glory. And plasma.", RebindableHotkeyEnum::skullGruntFuneralHotkey}},
		{ SkullEnum::IWHBYD,{ "IWHBYD", "Rare combat dialogue becomes more common.", RebindableHotkeyEnum::skullIWHBYDHotkey}},
		{ SkullEnum::Malfunction,{ "Malfunction", "Every time you respawn, a random element of your HUD is diabled.", RebindableHotkeyEnum::skullMalfunctionHotkey}},
		{ SkullEnum::Pinata,{ "Pinata", "Punching enemies makes them drop grenades. Beat them 'til candy comes out!", RebindableHotkeyEnum::skullPinataHotkey}},
		{ SkullEnum::ProphetBirthdayParty,{ "Prophet Birthday", "Punch that Prophet up to 11.", RebindableHotkeyEnum::skullProphetBirthdayPartyHotkey}},
		{ SkullEnum::Scarab,{ "Scarab", "All player-held weapons fire Scarab Gun beams.", RebindableHotkeyEnum::skullScarabHotkey}},
		{ SkullEnum::Sputnik,{ "Sputnik", "Mass of objects is decreased, making them more easily displaced.", RebindableHotkeyEnum::skullSputnikHotkey}},
		{ SkullEnum::Acrophobia,{ "Acrophobia", "The Corps: Now issuing rifles AND wings…", RebindableHotkeyEnum::skullAcrophobiaHotkey}},
	};

public:


	GUISkullToggle(GameState implGame, ToolTipCollection tooltip, std::string headingText, std::shared_ptr<SettingsStateAndEvents> settings, float leftMargin = 20.f)
		: IGUIElement(implGame, std::nullopt, tooltip), mLeftMargin(leftMargin), mHeadingText(headingText), mSettingsWeak(settings)
	{
		PLOG_VERBOSE << "Constructing GUISkullToggle, name: " << getName();



		this->currentHeight = GUIFrameHeightWithSpacing;

	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{

		auto mSettings = mSettingsWeak.lock();
		if (!mSettings) { PLOG_ERROR << "bad mSettings weak ptr"; return; }
		
		ImGui::BeginChild(mHeadingText.c_str(), { 500 - mLeftMargin, currentHeight - GUISpacing });


		constexpr ImGuiTreeNodeFlags treeFlags = startsOpen ? ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_FramePadding;
		headingOpen = ImGui::TreeNodeEx(mHeadingText.c_str(), treeFlags);
		renderTooltip();
		DEBUG_GUI_HEIGHT;
		if (headingOpen)
		{

			currentHeight = GUIFrameHeightWithSpacing * 2;


			// tell optional cheat service to update bit bools
			mSettings->updateSkullBitBoolCollectionEvent->operator()(mImplGame);
			ImGui::Text("Note: skull state is saved/loaded by checkpoints");
			
			if (mSettings->skullBitBoolCollectionInUse) // await update if necessary
			{
				mSettings->skullBitBoolCollectionInUse.wait(true);
			}
			
			// lock it
			ScopedAtomicBool lock(mSettings->skullBitBoolCollectionInUse);


			if (mSettings->skullBitBoolCollection.empty())
			{
				float thisElementHeight = GUIFrameHeightWithSpacing;
				currentHeight += thisElementHeight; // height of the error message
				ImGui::Text("Null skull pointers! See log");
			}
			else
			{
				float checkBoxElementHeight = GUIFrameHeightWithSpacing;

				currentHeight += GUIFrameHeightWithSpacing; // little extra height to account for table padding
				currentHeight += checkBoxElementHeight * std::ceil(mSettings->skullBitBoolCollection.size() / 2.f); // add height for amount of rows (skulls / 2 rounded up)

				ImGui::BeginTable("skullTable", 2); // return value is whether or not it's clipped

				// render each available skull toggle 
				for (auto& [skullEnumKey, bitBoolPointer] : mSettings->skullBitBoolCollection)
				{
					ImGui::TableNextColumn();

					// get render info ref
					auto& renderInfo = mSkullRenderInfo.at(skullEnumKey);

					// render hotkey
					hotkeyRenderer.renderHotkey(renderInfo.hotkeyEnum, 30);
					ImGui::SameLine();

					// render checkbox
					bool tempSkullValue = bitBoolPointer.operator const bool();
					if (ImGui::Checkbox(renderInfo.displayName.c_str(), &tempSkullValue)) // TODO: replace enum_name with display name via map
					{
						bitBoolPointer.set(tempSkullValue);
					}

					// render tooltip
					if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
					{
						ImGui::SetTooltip(renderInfo.toolTip.c_str());
					}
				}
				ImGui::EndTable();
				
				
			}
			


			ImGui::TreePop();
		}
		else
		{
			currentHeight = GUIFrameHeightWithSpacing;
		}
		ImGui::EndChild();


	}

	std::string_view getName() override { return mHeadingText; }

};