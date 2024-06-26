#pragma once
#include "pch.h"
#include "MCCState.h"
// h1 and h2 can look up trigger names dynamically, which means they can support mp and custom maps just fine
// but third gen (h3+) has a really complicated stringID system that I can't be bothered to figure out.
// The assembly guys did figure it out tho if you want to try
// Anyway instead we're just gonna hardcode it in a lookup table by level id
// I literally used assembly + a cheatengine lua script hooked to assembly to get these values lol

static inline std::map<LevelID, std::vector<std::string>> triggerNamesHardcoded =
{
	{LevelID::_map_id_halo3_sierra_117, std::vector<std::string>
		{
			"tv_fp1", "begin_zone_set:set_jungle_walk", "zone_set:set_jungle_walk", "begin_zone_set:set_transition_a", "zone_set:set_transition_a", "begin_zone_set:set_grunt_camp", "begin_zone_set:set_grunt_camp", "zone_set:set_grunt_camp", "begin_zone_set:set_path_a", "begin_zone_set:set_path_a", "begin_zone_set:set_path_a", "zone_set:set_path_a", "begin_zone_set:set_substation", "zone_set:set_substation", "begin_zone_set:set_path_b", "begin_zone_set:set_path_b", "begin_zone_set:set_path_b", "zone_set:set_path_b", "begin_zone_set:set_path_c", "zone_set:set_path_c", "begin_zone_set:set_dam", "zone_set:set_dam", "zone_set:set_dam", "tv_cc_jump_training", "tv_cc_01", "tv_cc_02", "tv_cc_03", "tv_cc_04", "tv_cc_05", "tv_cc_06", "tv_cc_07", "tv_cc_08", "tv_cc_09", "tv_cc_10", "tv_cc_11", "tv_enc_jungle_walk", "tv_jw_climb_out", "tv_010pa_right", "tv_010pa_left", "tv_jw_01", "tv_jw_02", "tv_jw_03", "tv_jw_04", "tv_jw_05", "tv_jw_06a", "tv_jw_07", "tv_jw_08", "tv_jw_09", "tv_jw_10", "tv_jw_11", "tv_jw_12", "tv_enc_grunt_camp", "tv_gc_01", "tv_gc_02", "tv_gc_03", "tv_gc_04", "tv_gc_05", "tv_gc_06a", "tv_gc_06b", "tv_gc_06c", "tv_gc_07a", "tv_enc_path_a", "tv_pa_01", "tv_pa_02", "tv_pa_03", "tv_pa_04", "tv_pa_05", "tv_pa_06", "tv_pa_07", "tv_pa_08", "tv_pa_09", "tv_enc_substation", "tv_ss_01", "tv_ss_02", "tv_ss_03", "tv_ss_04", "tv_ss_05", "tv_ss_06", "tv_ss_07", "tv_ss_08", "tv_ss_09", "tv_ss_10", "tv_ss_front_bldg_tops", "tv_ss_back_bldg_top", "tv_ss_back_upper_ledge", "tv_enc_path_b", "tv_pb_01", "tv_enc_brute_ambush", "tv_ba_01", "tv_ba_01_02", "tv_ba_02_03_04", "tv_ba_02_03_04_05", "tv_ba_03_04_05a", "tv_ba_03_04_05b", "tv_ba_03_04_05c", "tv_ba_ledge", "tv_enc_dam", "tv_dam_01", "tv_dam_02", "tv_dam_03", "tv_dam_04", "tv_dam_04_bottom", "tv_dam_05", "tv_dam_05_bottom", "tv_dam_06", "tv_dam_06_bottom", "tv_dam_07", "tv_dam_08", "tv_teleport_players", "tv_rec_cc", "tv_rec_jw", "tv_rec_gc", "tv_rec_pa", "tv_rec_ss", "tv_rec_pb", "tv_rec_ba", "tv_rec_dam", "kill_jungle_walk", "kill", "kill", "kill", "tv_gc_07b", "tv_gc_ini_01", "tv_gc_ini_02", "tv_ta_01", "tv_ta_02", "tv_ta_03", "tv_ta_04", "tv_ta_05", "tv_ta_06", "tv_gc_ini_03", "tv_dam_rescue", "tv_pb_02", "tv_pb_03", "tv_pb_04", "tv_pb_05", "tv_pb_06", "tv_tb_01", "tv_tb_02", "tv_tb_03", "tv_tb_04", "tv_ba_03", "tv_ss_fish_01", "tv_ss_fish_02", "tv_ss_fish_03", "tv_jw_fish_01", "tv_dam_fish_01", "tv_cc_birds_01", "tv_jw_birds_01", "tv_jw_fish_02", "tv_pa_rats_01", "tv_jw_06b", "tv_jw_rats_01", "tv_cortana_01", "tv_truth", "tv_dam_radio", "tv_ba_pelican", "tv_jw_out_of_bounds", "tv_gc_md_sleepers", "tv_pb_07", "begin_zone_set:set_path_a", "begin_zone_set:set_substation", "zone_set:set_path_c"
		}
	},

	{LevelID::_map_id_halo3_crows_nest, std::vector<std::string>
		{ 
			"hangar_a_phantom_trig", "hangar_a_front_fallback_trig", "hangar_a_start_trig", "hangar_a_door_entrance_trig", "evac_hangar_elevator01_trig", "evac_hangar_encounter01_trig", "exit_door_trig", "highway_a_start_trig", "cave_a_start_trig", "teleport_players_trig", "loop01_return_start_trig", "loop01_return_marine_trig", "loop01_return_marine02_trig", "command_center_intro_trig", "motor_pool_start_trig", "sewer01_trig", "sewer02_trig", "zone_set:020_00_04_08_encounter", "motor_pool_player_vol02", "barracks_start_trig", "barracks_player_vol02", "barracks_marine_end_trig", "barracks_player_vol01", "evac_hangar_start_trig", "barracks_player_vol04", "self_destruct_start_trig", "motor_pool_player_vol03", "motor_pool_player_vol02a", "motor_pool_player_vol02b", "cortana_highway_enc_trig", "self_destruct_player_vol02", "self_destruct_bomb_trig", "self_destruct_bomb_placement_trig", "sewer_bugger01_trig", "sewer_bugger02_trig", "sewer_bugger03_trig", "sewer_bugger04_trig", "sewer_bugger06_trig", "sewer_bugger05_trig", "cave_a_marine_1", "command_exit_trig", "cave_a_hwy_marines_trig", "cave_a_hwy_marines01_trig", "evac_hangar_encounter02_trig", "evac_hangar_encounter03_trig", "evac_hangar_encounter04_trig", "evac_hangar_encounter05_trig", "evac_hangar_encounter06_trig", "evac_hangar_encounter07_trig", "evac_hangar_encounter08_trig", "kill", "kill", "kill", "kill", "barracks_player_vol03", "top_elevator_trigger", "bugger_intro_follow_trig", "hangar_a_finish_trig", "motor_pool_save00", "motor_pool_save01", "motor_pool_save02", "motor_pool_save04", "motor_pool_save03", "cortana_highway_var_trig01", "cortana_highway_var_trig02", "cortana_highway_var_trig03", "motor_pool_upper_trigger", "command_center_terminal_trig", "cave_a_dia01_trig", "cave_a_dia02_trig", "cave_a_dia03_trig", "cave_a_dia04_trig", "vol_cortana_hall_start", "cave_a_hwy_marines02_trig", "cave_a_hwy_marines03_trig", "cave_a_hwy_briefing_trig", "sewer_start_trig", "sewer_vignette_trig", "evac_hangar_begin_encounter", "evac_hangar_control_room_trig", "cortana_highway_end_dialog", "self_destruct_bomb_near_placement_trig", "command_center_intro_marines", "cortana_highway_start_trig", "barracks_vignette_start_trig", "barracks_marine05_rescue_trig", "loop01_return_all_in_trig", "kill", "armory_coop_trig01", "loop01_return_all_in_trig01", "loop01_return_all_in_trig02", "loop01_return_all_in_trig03", "loop01_return_all_in_trig04", "highway_a_mid_trig", "cave_a_hwy_marines00_trig", "hangar_a_var02", "loop01_return_bugger_trig", "hangar_a_return_start_trig", "cave_a_door_repair_trig", "base_exit", "hangar_a_coop_trig", "sewer_obj_trig01", "sewer_obj_trig02", "barracks_obj_end", "cave_a_entry_trig", "g_command_center_trig_a", "g_command_center_trig_b", "g_command_center_trig_c", "g_cave_a_trig_a", "g_cave_a_trig_b", "g_hallway_a_trig_a", "g_hallway_a_trig_b", "g_hallway_a_trig_c", "g_hallway_a_trig_d", "g_highway_trig_a", "g_highway_trig_b", "g_hallway_b_trig_a", "g_hangar_a_trig_a", "g_motor_pool_trig_a", "g_motor_pool_trig_b", "g_motor_hallway_trig_a", "g_sewer_trig_a", "g_evac_bottom_trig_a", "g_evac_bottom_trig_b", "g_evac_bottom_trig_c", "g_evac_top_trig_a", "g_cortana_highway_trig_a", "g_barracks_trig_a", "g_barracks_trig_b", "g_barracks_trig_c", "g_cortana_highway_trig_b", "g_cortana_highway_trig_c", "loop02_begin_sgt_trig", "highway_a_rumble_trig", "evac_hangar_joh_event_trig", "flock_sky_battle01", "flock_sky_battle02", "hangar_a_return_mid_trig", "hangar_a_return_right_trig", "hangar_a_return_left_trig", "hangar_a_return_back_trig", "jet_pack_door_close_trig", "hangar_a_var03", "cortana_highway_turret_end", "bugger_scary_trig", "hangar_a_entrance_trig", "kill_hangar_a", "g_hallway_a_trig_e", "base_blow_up_trig", "flock_sky_battle03", "flock_sky_battle04", "flock_sky_battle05", "evac_hangar_encounter06_1_trig", "zone_set:020_00_01_encounter", "begin_zone_set:020_00_01_encounter", "zone_set:020_01_03_encounter", "begin_zone_set:020_01_03_encounter", "zone_set:020_00_04_05_encounter", "begin_zone_set:020_00_04_05_encounter", "begin_zone_set:020_04_05_06_encounter", "zone_set:020_05_06_07_encounter", "begin_zone_set:020_05_06_07_encounter", "zone_set:020_05_06_07_encounter", "begin_zone_set:020_06_08_04_encounter", "begin_zone_set:020_00_04_08_encounter", "zone_set:020_06_08_04_encounter", "vol_cc_left", "vol_cc_right", "vol_bomb_scene_02", "vol_cc_return", "vol_near_cc_exit", "vol_arbiter_vanish", "vol_cave_a_rats_02", "vol_cave_a_rats_01", "vol_cave_a_rats_03", "vol_cave_a_rats_04", "vol_cave_a_rats_05", "vol_cave_a_rats_06", "vol_cave_a_rats_07", "vol_cave_a_rats_08", "vol_hall_a_rats_01", "vol_hall_a_rats_02", "vol_hall_a_rats_03", "vol_hangar_a_rats", "vol_hangar_a_rats_01", "vol_hangar_a_rats_02", "vol_hangar_a_rats_03", "vol_hangar_a_rats_04", "vol_hangar_a_rats_05", "vol_motorpool_rats_01", "vol_motorpool_rats_02", "vol_evac_rats_01", "vol_evac_rats_02", "vol_evac_rats_03", "vol_barracks_rats_01", "begin_zone_set:020_00_01_return", "zone_set:020_00_01_return", "begin_zone_set:020_00_01_exit_encounter", "zone_set:020_00_01_exit_encounter", "zone_set:020_00_01_exit_encounter", "zone_set:020_01_03_exit_encounter", "begin_zone_set:020_01_03_exit_encounter", "hangar_a_upstairs_trig", "hangar_a_pelican_volume", "highway_second_part_trig", "cortana_exit_run_trig", "barracks_arbiter_trig", "zone_set:020_01_03_encounter", "blow_up_safe_zone", "bugger_flock_vol", "bugger_flock_vol02", "bugger_flock_vol04", "bugger_flock_vol03", "bugger_kill", "begin_zone_set:020_03_exit", "zone_set:020_03_exit", "cool_cam_trig01", "cool_cam_trig02", "cool_cam_trig03", "cool_cam_trig07", "cool_cam_trig08", "cool_cam_trig09", "cool_cam_trig10", "cool_cam_trig04", "cool_cam_trig05", "cool_cam_trig06", "easter_egg_trig01", "cortana_moment_sewer_trig", "loop01_return_miranda_pa_trig", "vt_barracks02_trig", "vt_barracks03_trig", "evac_hangar_kill_trig", "zone_set:020_04_05_06_encounter", "zone_set:020_06_08_04_encounter", "kill_evac_elevator", "zone_set:020_06_08_04_encounter", "self_destruct_coop_teleport"
		}
	},

	{LevelID::_map_id_halo3_tsavo_highway, std::vector<std::string>
		{ 
			"zone_set:bc:*", "zone_set:bc:*", "zone_set:cd:*", "zone_set:cd:*", "zone_set:de:*", "zone_set:ef:*", "zone_set:ef:*", "zone_set:f:*", "zone_set:f", "zone_se", "zone_se", "zone_se", "zone_se", "zone_se", "zone_se", "zone_se", "zone_se", "zone_set:f:*", "tv_gc_bsp010", "tv_gc_bsp020", "tv_gc_bsp030", "tv_gc_bsp030b", "tv_gc_bsp040", "tv_gc_bsp040b", "tv_gc_bsp050a", "tv_gc_bsp050", "tv_gc_bsp05", "tv_gc_bsp05", "tv_gc_bsp05", "tv_gc_bsp0", "tv_gc_bsp0", "tv_gc_bsp0", "tv_gc_bsp0", "tv_gc_bsp0", "tv_gc_bsp0", "tv_gc_bsp0", "tv_gc_bsp0", "tv_gc_bsp050", "tv_drive_oc2", "tv_drive_oc3", "tv_drive_oc4", "tv_drive_oc5", "tv_drive_oc6", "tv_drive_oc7", "tv_drive_oc8", "tv_drive_flee_a", "tv_drive_flee_b", "tv_drive_fl", "tv_drive_fl", "tv_drive_fl", "tv_drive_fl", "tv_drive_fl", "tv_drive_fl", "tv_drive_fle", "tv_crash_oc2", "tv_crash_oc3", "tv_crash_oc4", "tv_crash_oc5", "tv_crash_oc6", "tv_crash_scene1", "tv_bridge_oc1", "tv_bridge_oc2", "tv_bridge_oc3", "tv_bridge_oc4", "tv_bridge_oc5", "tv_bridge_oc6", "tv_garage_oc1", "tv_garage_oc2", "tv_garage_oc3", "tv_garage_oc4", "tv_garage_near", "tv_garage_checkpoint01", "tv_tether_oc1", "tv_tether_oc2", "tv_tether_oc3", "tv_round_oc1a", "tv_round_oc1", "tv_round_oc2", "tv_round_oc3", "tv_round_oc4", "tv_round_oc", "tv_round_oc", "tv_round_oc", "tv_round_oc", "tv_round_oc", "tv_round_oc", "tv_round_oc", "tv_round_oc4", "tv_elev_medic01", "tv_crash_flock", "tv_bridge_flock", "tv_pond_flock", "tv_garage_pelican1", "tv_garage_pelican2", "tv_pond_banshee", "tv_elev_medic02", "zone_set:ab:*", "begin_zone_set:bc:*", "begin_zone_set:cd:*", "begin_zone_set:de:*", "begin_zone_set:ef:*", "zone_set:f:*", "tv_elev_door", "tv_drive_vista", "tv_round_save", "tv_exit_flock", "tv_exit_enemy_check", "tv_crash_dialog01", "tv_crash_dialog02", "tv_crash_dialog_mombasa", "tv_crash_truth", "tv_round_truth", "tv_exit_truth", "tv_round_vista1", "tv_round_vista2", "tv_elev_check", "tv_elev_check1", "tv_drive_object_manage", "tv_exit_pelican_dropoff", "tv_garage_pelican_kill", "tv_cav_camera", "tv_exit_pelican_kill", "tv_exit_pe", "tv_exit_pelican_ki", "tv_garage_02b", "tv_elev_nokill", "tv_exit_enemy_check2"
		}
	},

	{LevelID::_map_id_halo3_the_storm, std::vector<std::string>
		{
			"vol_intro_start", "vol_intro_flyby", "vol_drive_intro_end", "vol_dri", "vol_dri", "vol_drive_intro_end", "vol_tank_room_a_back", "vol_tan", "vol_tank_room_a_bac", "vol_factory_a_room02_entry", "vol_tank_room_a_right", "vol_tank_room_a_left", "vol_tank_room_a_exit", "vol_faa_clear_01", "vol_faa_clear_02", "vol_lakebed_a_start", "vol_lakebed_a_high_start", "vol_lakebed_a_low_start", "vol_lakebed_a", "vol_lakebed_a_troop_init", "vol_lakebed_a_bridge", "vol_lakebed_a_end", "vol_lak", "vol_lakebed_a_end", "vol_factory_b_start", "vol_factory_b_center", "vol_factory_b_upperlevel", "vol_factory_b_upperlevel02", "vol_factory_b_upperlevel03", "vol_factory_b_tunnel_end", "vol_factory_b_end", "vol_fab_entryroom", "vol_pump_room_b_up_a", "vol_pump_room_b_up_b", "vol_lakebed_b_start", "vol_lakebed_b_ledge", "vol_lakebed_b_backhalf", "vol_lakebed_b_center", "zone_set:intro_faa", "begin_zone_set:ware", "vol_drive_factory_a_end", "vol_drive_lakedbed_a_end", "vol_drive_factory_b_end", "vol_drive_lakebed_b_end", "vol_ark_opens_persp", "vol_intro_temp", "vol_warehouse_brute_flank", "vol_warehouse_brutes02", "vol_warehouse_start", "vol_warehouse_hunters01", "vol_warehouse_brute_run", "vol_lake_a_doorcheck", "vol_lake_a_bed", "vol_lake_a_topback", "vol_lake_a_topback02", "vol_factory_b_tunnel", "vol_factory_b_init03", "vol_scarab_crane_left", "vol_scarab_crane_right", "vol_lakebed_b_crane_left", "vol_lakebed_b_crane_right", "vol_intro_hog_msg", "vol_intro_briefing", "vol_intro_troop_end", "vol_intro_troop_end", "vol_intro_troo", "vol_intro_", "vol_intro_troop_end", "vol_lake_b_doorcheck", "vol_lakebed_b_backwall", "lakebed_b_garbage", "vol_lakebeda_wraithblock", "vol_lakebed_b_finalinf", "vol_scarab_back", "vol_warehouse_brutes01", "vol_warehouse_brutes02b", "vol_worker_start", "vol_ware_nav_exit", "vol_worker_entry_side", "vol_banshees02_start", "vol_worker_middle", "vol_worker_middle_start", "vol_worker_entry", "vol_warehouse_marine01", "vol_bfg_advance", "vol_bfg_cov_right", "vol_bfg_cov_left", "vol_bfg_marine02", "vol_bfg_entry", "vol_bfg_cov_right02", "vol_bfg_init02", "vol_bfg_middle_hill", "vol_bfg_sniperstart", "vol_bfg_init", "vol_office_start", "vol_office_start02", "vol_office_start03", "vol_factory_b_buginit", "vol_lakebed_b_entryprox", "vol_factory_b_bug_garbage", "vol_bfg_gamesave01", "vol_warehouse_saves01", "vol_warehouse_saves02", "vol_bfg_marine01", "vol_warehouse_fallback03", "vol_lake_a_rl", "vol_factory_b_buginit02", "vol_faa_button02", "factory_b_garbage", "lakebed_a_garbage", "factory_a_garbage", "warehouse_garbage", "vol_lakebed_b_persp", "vol_lakebed_a_flock01", "vol_lakebed_b_flock01", "vol_worker_flock01", "vol_intro_flock01", "vol_lakebed_b_end_advance", "vol_faa_center", "vol_office_doorclose", "vol_warehouse_init", "zone_set:faa_lakea", "zone_set:fab_lakeb", "begin_zone_set:scarab", "vol_intro_go", "vol_intro_go", "vol_intro_go", "vol_warehouse_hunters03", "kill_bfg_cliff", "vol_warehouse_hunters02", "begin_zone_set:intro_faa", "zone_set:ware", "begin_zone_set:bfg", "begin_zo", "begin_zone_set:bfg", "begin_zone_set:fab_lakeb", "vol_bfg_brutes01", "vol_worker_bfgtest", "tv_cortana_01", "vol_faa_upperflank", "begin_zone_set:ware_worker", "zone_set:ware_worker", "zone_set:bfg", "tv_truth_worker", "kill_all_lakebed_a", "vol_lakebed_a_flock_ark01", "vol_lakebed_b_phantom_flock", "vol_lakebed_a_phantom_flock", "tv_cortana_03", "vol_lake_a_surf", "vol_lake_b_surf", "vol_faa_coop_teleport", "vol_fab_coop_teleport01", "vol_fab_coop_teleport02", "vol_office_coop_teleport", "vol_office_coop_teleport", "vol_office_coop_teleport", "vol_office_coop_telep", "vol_lake_b_surf02", "vol_bfg_garbage", "vol_lake_b_backup_area", "vol_scarab_coop_teleport01", "vol_scarab_coop_teleport02", "vol_scarab_coop_test", "vol_lake_a_saves01", "kill_lakebed_b_extra", "vol_factory_b_buginit03", "vol_warehouse_backhalf", "kill_bfg_cin_start", "vol_lake_a_center_saves"
		}
	},

	{LevelID::_map_id_halo3_floodgate, std::vector<std::string>
		{
			"fp_01", "volume_lakebed_b", "volume_lakebed_a", "zone_set:set_warehouse", "zone_set:set_warehouse", "zone_set:set_trans_a", "zone_set:set_trans_a", "zone_set:set_lakebed_b", "zone_set:set_lakebed_b", "zone_set:set_factory_arm_b", "zone_set:set_factory_arm_b", "zone_set:set_lakebed_a", "zone_set:set_lakebed_a", "zone_set:set_lakebed_a", "zone_set:set_lakebed_a", "zone_set:set_lakebed_a", "zone_set:set_lakebed_a", "zone_set:set_lakebed_a", "zone_set:set_flood_ship", "zone_set:set_cin_outro", "begin_zone_set:set_warehouse", "begin_zone_set:set_trans_a", "begin_zone_set:set_lakebed_b", "begin_zone_set:set_lakebed_b", "begin_zone_set:set_factory_arm_b", "begin_zone_set:set_factory_arm_b", "begin_zone_set:set_lakebed_a", "begin_zone_set:set_lakebed_a", "begin_zone_set:set_flood_ship", "begin_zone_set:set_cin_outro", "tv_wt_01", "tv_wt_02", "tv_wt_03", "tv_wt_04", "tv_wt_05", "tv_wt_06", "tv_wt_07", "tv_wt_08", "tv_wt_09", "tv_wt_10", "tv_wt_11", "tv_wt_12", "tv_wt_13", "tv_wt_14", "tv_enc_warehouse", "tv_wh_01", "tv_wh_02", "tv_wh_03", "tv_wh_04", "tv_wh_05", "tv_wh_06", "tv_wh_07", "tv_wh_08", "tv_wh_09", "tv_wh_10", "tv_wh_11", "tv_wh_12", "tv_wh_13a", "tv_enc_lakebed_b", "tv_lb_01", "tv_lb_02", "tv_lb_03", "tv_lb_04", "tv_lb_05", "tv_lb_06", "tv_lb_07", "tv_lb_08", "tv_lb_09", "tv_lb_10", "tv_enc_factory_arm_b", "tv_fb_01", "tv_fb_02", "tv_fb_03", "tv_fb_04", "tv_fb_05", "tv_fb_06", "tv_fb_07", "tv_enc_lakebed_a_01", "tv_enc_lakebed_a_02", "tv_la_01a", "tv_la_01b", "tv_la_01c", "tv_la_02", "tv_la_03", "tv_la_04", "tv_la_05", "tv_la_06", "tv_la_07", "tv_la_08", "tv_la_09", "tv_enc_floodship", "tv_ta_01", "tv_ta_02", "tv_ta_03", "tv_ta_04", "tv_ta_05", "tv_ta_06", "tv_ta_07", "tv_rec_wt", "tv_rec_wh_01", "tv_rec_wh_02", "tv_rec_wh_03", "tv_rec_wh_04", "tv_rec_lb_01", "tv_rec_lb_02", "tv_rec_lb_03", "tv_rec_lb_04", "tv_rec_fb", "tv_rec_la_01", "tv_rec_la_02", "tv_rec_la_03", "kill_volume_workertown", "tv_gravemind_01", "tv_gravemind_02", "begin_zone_set:set_factory_arm_b", "tv_cortana_01", "tv_wh_marine", "tv_wh_carrier", "tv_wh_music", "tv_fs_music2", "tv_wh_13b"
		}
	},

	{LevelID::_map_id_halo3_the_ark, std::vector<std::string>
		{
			"begin_zone_set:070_000_005", "zone_set:070_000_005", "begin_zone_set:070_000_005_010", "zone_set:070_000_005_010", "begin_zone_set:070_005_010:*", "zone_set:070_005_010:*", "begin_zone_set:070_010_011:*", "zone_set:070_010_011:*", "begin_zone_set:070_010_020:*", "zone_set:070_010_020:*", "zone_set:070_010:*", "begin_zone_set:070_020_030", "zone_set:070_020_030", "zone_set:070_020_030", "begin_zone_set:070_030_040:*", "zone_set:070_030_040:*", "zone_set:070_040_050_071", "begin_zone_set:070_050_060_080_071", "zone_set:070_050_060_080_071", "begin_zone_set:070_060_070_080_071", "zone_set:070_060_070_080_071", "begin_zone_set:070_080", "vol_bb_bottom_floor", "vol_bb_middle", "vol_bb_canyon", "vol_bb_arrival", "vol_bb_start", "vol_bb_canyon_start", "vol_bb_canyon_end", "vol_bb_1st_floor_start", "vol_bb_1st_floor", "vol_bb_2nd_floor_start", "vol_bb_2nd_floor", "vol_bb_3rd_floor_start", "vol_bb_4th_floor_start", "vol_bb_base", "vol_f5_start", "vol_f3_p2_advance_1", "vol_f3_p2_advance_2", "vol_f3_p2_advance_3", "vol_f4_1st_floor", "vol_f4_2nd_floor", "vol_f4_ambush_right", "vol_f4_start", "vol_f4_right", "vol_f4_left", "vol_f5_1st_floor", "vol_f3_cinematic", "vol_f5_2nd_floor", "vol_f1_front", "vol_f3_left", "vol_f3_right", "vol_f2_advance_0", "vol_f2_advance_1", "vol_f2_advance_2", "vol_f2_advance_3", "vol_f2_advance_4", "vol_f2_advance_5", "vol_f3_start", "vol_f1_start", "vol_f5_left", "vol_f4_deaf_zone", "vol_f2_end_1st_floor", "vol_bb_base_back", "vol_bb_canyon_middle", "vol_bb_infantry_zone", "vol_abb_pelican_lz", "vol_f1_stairway", "vol_f1_left", "vol_f1_right", "vol_f1_back", "vol_f2_start", "vol_f3_end_of_room", "vol_f3_advance_0", "vol_f3_advance_1", "vol_f3_p2_advance_4", "vol_f4_right_room", "vol_f5_stairs_right", "vol_f5_mc_low_left", "vol_f5_mc_high_left", "vol_f5_mc_low_right", "vol_f5_mc_high_right", "vol_bb_dropdown", "vol_f4_left_room", "vol_f4_ambush_left", "vol_f5_right", "vol_bb_bottom_floor_left", "vol_bb_bottom_floor_right", "vol_bb_scarab", "vol_lb_guardian_room", "vol_lb_final_room", "vol_bb_base_entrance", "vol_abb_storm_in", "vol_abb_shaft_entrance", "vol_f1_dive", "vol_f1_near_door", "vol_f3_dialog_start", "vol_f3_p1_advance_1", "vol_f3_p1_advance_4", "vol_f3_p1_advance_5", "vol_f3_p1_advance_6", "vol_p3_gs_intro", "vol_f3_just_outside", "vol_f3_outside", "vol_f3_p2_advance_5", "vol_f4_near_exit", "vol_f5_stairs_left", "vol_f5_viewpoint", "vol_bb_vd_start", "vol_kill_f3_0", "vol_kill_f3_1", "vol_kill_f3_2", "vol_kill_f3_3", "vol_kill_f3_4", "vol_kill_f3_5", "vol_bb_recycle", "vol_f1_recycle", "vol_f2_recycle", "vol_f3_recycle", "vol_f4_recycle", "vol_f2_all", "vol_f3_all", "vol_f3_stairway", "vol_f4_all", "vol_f4_stairway", "vol_f5_outside", "vol_bb_lock_lb_door", "vol_bb_all", "vol_f4_music_start", "vol_lb_scarab_view", "vol_ex_start", "vol_ex_ground", "vol_ex_out_cave", "vol_ex_leaving_1", "vol_ex_longsword", "vol_ex_left_ledge", "vol_lz_exit_intro", "vol_lz_middle_explore", "vol_b1_cave_1", "vol_b1_cave_0", "vol_b1_left", "vol_b1_rear", "vol_b2_cave_0", "vol_aw_start", "vol_aw_cave", "vol_aw_middle", "vol_aw_pass_bridge", "vol_la_going_down", "vol_la_bridge", "vol_la_cave", "vol_la_before_trench", "vol_la_trench", "vol_la_mid_trench", "vol_la_after_cave", "vol_la_before_cave", "vol_ex_near_trench", "vol_ex_exit_trench", "vol_dw_after_trench", "vol_dw_entrance", "vol_la_trench_end_0", "vol_la_trench_end_1", "vol_la_trench_end_2", "vol_dw_on_scarab", "vol_dw_spawn_zone", "vol_dw_later_half", "vol_fp_entrance", "vol_fp_front_door", "vol_sd_entrance", "vol_sd_begin", "vol_sd_middle", "vol_fl_bridge", "vol_fl_up", "vol_fl_down", "vol_fl_entrance", "vol_lb_entrance", "vol_lb_switch_room", "vol_lb_near_guardian_room", "vol_lb_begin", "vol_lb_center", "vol_lb_sentinel_disappear", "vol_lb_recycle", "vol_fl_all", "vol_fl_tank_exit", "vol_fl_sgt_briefing", "vol_dw_down", "vol_dw_going_up", "vol_lz_recycle", "vol_b1_recycle", "vol_b2_recycle", "vol_fp_recycle", "vol_ex_recycle", "vol_aw_recycle", "vol_fl_recycle", "vol_la_recycle", "vol_dw_recycle", "vol_sd_recycle", "vol_aw_watchtower_0", "vol_la_watchtower_1", "vol_lb_teleport", "vol_flock_bowls", "vol_lb_teleport_2", "vol_lb_mid_entrance", "vol_dw_teleport_allies", "vol_lb_allies_wait", "vol_b2_cave_1", "vol_fl_tanks_on_ground", "kill_volume_bowls", "vol_dw_near_door", "vol_b2_ramp_0", "vol_b2_ramp_1", "vol_fp_all", "vol_fp_before_door", "vol_ex_mid_trench", "vol_sd_near_wall", "vol_la_recycle_cave", "vol_b1_middle", "vol_la_p2_begin", "vol_la_p1", "vol_bb_scarab_top", "vol_ex_sd_area", "vol_ex_sd_encounter", "vol_bb_higher_floors", "vol_ex_teleport", "vol_flock_ex_phantom", "vol_flock_ex_sky_battle_left", "vol_flock_ex_sky_battle_over_left", "vol_flock_ex_sky_battle_right", "vol_flock_aw_sky_battle", "vol_b2_cave_2", "vol_flock_ex_sky_battle_over_right", "vol_flock_f3_sky_battle_right", "vol_flock_f3_sky_battle_left", "vol_flock_f5_sky_battle", "vol_fp_entrance_cave", "vol_f3_p1_advance_2", "vol_f3_p1_advance_3", "vol_f3_interior", "vol_flock_bowls_hornet", "vol_b2_ramp_base", "vol_f5_challenge_ring", "vol_f5_near_ring", "vol_f5_near_pelican", "vol_f5_cin_cleanup", "vol_f4_advance_0", "vol_f4_advance_1", "vol_f4_exit", "vol_f4_after_start", "vol_bb_ghost_spawn", "vol_ex_leaving_0", "vol_ex_near_crash", "vol_ex_exit_mauler", "vol_dw_bridge", "vol_dw_advance", "vol_sd_bridge", "vol_lb_spawn_sentinels", "vol_lb_gs_open_door", "vol_dw_almost_in", "kill_volume_f5_bottom", "vol_b1_watchtower", "vol_ex_corridor", "vol_f2_near_terminal", "vol_la_teleport_if_in", "vol_lb_kill_volume", "vol_ex_near_entrance", "vol_ex_exit_mauler_large", "vol_sd_mid_bridge", "vol_b1_middle_radial", "vol_b2_middle", "vol_b2_charge", "vol_ex_exit", "vol_ex_after_trench", "vol_ex_teleport_2", "vol_ex_leaving_2", "vol_b1_down", "vol_lz_end", "vol_lz_near_cave", "kill_volume_aw", "vol_f3_exterior", "vol_aw_watchtower_1", "vol_ex_part_2", "vol_ex_near_crashed_phantom", "vol_f3_p1_advance_4_5", "vol_dw_left", "vol_aw_first_half", "vol_ex_after_longsword", "vol_sd_begin_catch_all", "vol_f2_terminal", "vol_b2_kill_players", "vol_la_kill_players", "vol_dw_kill_players_0", "vol_dw_kill_players_1", "vol_bb_kill_players", "vol_abb_kill_players", "vol_f3_kill_players", "vol_lz_kill_players", "vol_sd_need_pelican", "vol_ex_kill_players", "vol_b1_activate_halogram", "vol_bb_scarab_bottom", "vol_aw_hornet_combat_zone", "vol_aw_all", "vol_fl_right_clift", "vol_f3_p1_advance_0", "vol_f3_p2_spawn_reinf_0", "vol_f3_p2_spawn_reinf_1", "vol_f5_teleport", "vol_ex_checkpoint_before_sd", "vol_fp_cave", "vol_ex_sentinel_exit", "vol_lb_after_door", "vol_abb_halfway_ramp", "vol_f1_start_player", "vol_ex_later_half", "vol_abb_before_ramp", "vol_b1_cave_out", "vol_ex_before_trench", "vol_abb_spawn_base", "vol_abb_near_base", "vol_fl_teleport_players", "kill_volume_bb_0", "vol_abb_near_top", "vol_f5_outside_no_stairs", "kill_volume_bb_1", "vol_ex_teleport_3", "vol_f5_pass_truth", "begin_zone_set:070_080", "begin_zone_set:070_080", "zone_set:070_080", "vol_dw_turn_corner", "vol_f5_outside_left", "vol_f5_outside_right", "kill_volume_f5_right", "kill_volume_f5_left", "kill_volume_f5_back"
		}
	},

	{LevelID::_map_id_halo3_the_covenant, std::vector<std::string>
		{
			"begin_zone_set:set_cell_a_int:*", "zone_set:set_cell_a_int", "vol_beach_creekflank", "vol_beach_area02_start", "vol_lock_a01_start", "vol_core_a01_right", "vol_core_a01_end", "vol_core_a01_left", "vol_cell_a_wraith", "vol_cell_a_center", "vol_lock_a01b_start", "vol_cell_a_start", "vol_cell_a_bansheespawn", "vol_lock_a01b_backhall", "vol_lock_a01b_init", "vol_cell_b_start", "vol_scrarab_advance", "vol_crater_advance01", "vol_scarab01_radius", "vol_scarab02_radius", "vol_ring01_bottom", "vol_ring01_back", "kill_vol_ring", "vol_ring01_start", "vol_ring02_init", "vol_ring02_start", "vol_ring02_middle", "vol_ring03_init", "vol_ring02_back", "vol_ring01_back", "vol_cell_c_start", "vol_cell_c_middle", "vol_beach_start", "begin_zone_set:set_beach:*", "begin_zone_set:set_cell_a_int", "begin_zone_set:set_cell_a_int02", "vol_tankrun_start", "vol_tankrun_turn02", "kill_vol_tankrun", "vol_lock_a02_init", "zone_set:set_cell_c_int02", "vol_lock_c02_init", "vol_cell_b_init", "vol_lock_c01_start", "vol_beach_area03_start", "vol_cell_a_init", "vol_cell_a_newhog", "vol_lock_c04_floor", "vol_lock_c04_exit", "vol_lock_c04_exit02", "vol_cell_c_towerbase", "begin_zone_set:set_cell_c_int:*", "vol_crater_start", "vol_cell_a_pathend", "vol_cell_a_lockstart", "vol_ring03_end", "ring03_fps", "ring04_cortana01", "ring04_cortana02", "vol_ringex04_end", "vol_cell_a_aaprox", "vol_cell_a_ghostguard", "vol_tankrun_turn03", "vol_aa_unleash_high", "vol_aa_unleash_low", "vol_crater_garbage", "vol_cell_c_init_old", "vol_tankrun_banshees", "vol_crater_ghostview01", "begin_zone_set:set_waterfront:*", "vol_cell_c_init", "kill_vol_cell_b", "vol_cell_a_saves01", "vol_beach_odst_engage", "vol_cell_b_hornet_pickup_old", "vol_cell_b_objectives", "vol_cell_b_end", "vol_lock_c01_right", "vol_lock_c01_left", "vol_lock_c_start", "vol_lock_c01_back", "vol_lock_c01b_start", "vol_lock_c01b_bugger_end", "vol_lock_c01b_backhall", "vol_lock_c_ext_start", "vol_cell_c_ext01", "vol_cell_c_tankvol", "vol_tankrun01_start", "vol_tankrun01_cov01", "vol_tankrun01_migrate", "vol_crater_end01", "vol_tapestry_start", "vol_flood02_start", "vol_patha_choppers", "vol_patha_hoglink01", "vol_patha_flyby", "vol_lock_a03_start", "vol_lock_a03_follow", "vol_lock_a03_migrate", "vol_lock_a01_saves", "vol_lock_a02_saves", "vol_lock_c01b_bypass", "vol_lock_c01_saves", "vol_lock_c02_saves", "vol_cell_c_tankdrop", "vol_ringex03_halfway", "vol_ringex02_init", "vol_ringex02_halfway", "vol_cell_c_brutes", "vol_lock_c01_objs", "vol_beach_saves", "vol_cell_c_airfall02", "vol_cell_a_insight", "vol_cell_c_airfall", "vol_lock_c_elevator_test", "vol_lock_a_elevator_test", "beach_cella_locka_garbage", "lock_a_garbage", "vol_cell_a_towerexit", "vol_beach_shoreflank", "vol_beach_flock01", "vol_beach_flock02", "vol_beach_flock_pelicans01", "vol_crater_saves01", "vol_crater_flock01", "vol_crater_flock02", "vol_cell_c_brute_spawn", "vol_crater_doorclose", "vol_cella_flock01", "vol_cell_b_hornet_pickup", "vol_cell_a_tunnel", "vol_crater_entry_door_close", "vol_ice_start", "vol_crater_main", "ridge_garbage", "beach_garbage", "vol_cell_a_all", "vol_lock_c01b_bugger_end02", "vol_cell_b_towerclear", "vol_ringex01_init", "lock_c_garbage", "waterfront_garbage", "cell_c_garbage", "vol_tapestry_migrate", "vol_ice_start02", "vol_ice_left_behind", "vol_ice_renew_check", "vol_ice_middle", "ice_garbage", "vol_tank_wraith_low_cs", "vol_crater_vehicle_renew", "vol_ridge_renew_check", "ring03_swarm", "vol_lock_a_elevator_bottom", "vol_patha_hoglink02", "vol_patha_hoglink03", "vol_lock_c_elevator_bottom", "vol_tank_saves01", "vol_ringex03_end", "vol_ringex02_halfway02", "vol_ringex02_end", "vol_ringex01_halfway", "vol_cellc_flock01", "vol_beach_creekflank02", "vol_beach_first_center", "zone_set:set_cell_c_exit", "kill_vol_cell_c", "vol_beach_hog_area", "beach_pelican_drop_garbage", "vol_ring01_init", "zone_set:set_beach:02", "zone_set:set_cell_a_int02", "zone_set:set_waterfront", "vol_cella_flock02_fish01", "vol_cella_flock03_fish02", "vol_cella_flock04_fish03", "vol_ridge_garbage01", "vol_landing01_garbage", "vol_landing02_garbage", "vol_crater_hog_respawn", "vol_crater_kill_scorpion", "vol_scarab02_top", "vol_scarab01_top", "zone_set:set_cell_c_int", "begin_zone_set:set_crater:*", "zone_set:set_crater", "begin_zone_set:set_citadel_tapestry", "zone_set:set_citadel_tapestry", "begin_zone_set:set_cell_c_int02", "begin_zone_set:set_cell_c_exit", "zone_set:set_cell_ice:a", "zone_set:set_cell_ice:b", "begin_zone_set:set_cell_ice", "vol_100_title3", "vol_tankrun01_ghosts01", "vol_beach_banshee_fight", "path_a_game_save01", "vol_beach_marine_initadv", "vol_ringex03_jump01", "vol_ringex03_start", "vol_ringex03_com03", "vol_ringex02_jump01", "vol_ringex01_end", "vol_beach_marine_hilladv", "vol_cell_a_maulter_spawn", "vol_lock_a01_migrate", "vol_cell_b_halfway", "vol_tankrun01_vehicle_drop", "vol_lock_a01b_end", "tv_cortana_01", "tv_cortana_02", "tv_cortana_03", "begin_zone_set:set_beach:old", "zone_set:set_beach:01", "vol_cell_b_island", "begin_zone_set:set_citadel_ring02", "zone_set:set_citadel_ring02", "begin_zone_set:set_halo", "vol_lock_a01_bypass", "vol_crater_hornet_drop", "vol_crater_hornet_drop02", "vol_cell_c_arbiter_exit_test", "vol_ringex02_fore_adv", "vol_crater_flock_pelican01", "vol_crater_flock_phantom01", "vol_ring03_cheat", "arbiter_exit_surf", "vol_crater_tapestry_teleport", "kill_vol_ring_end", "vol_cell_c_arbiter_exit_test02", "vol_cell_a_vehicle_adv", "vol_skull_beta", "vol_skull_epsilon", "vol_skull_eta", "vol_skull_alpha", "vol_skull_delta", "vol_skull_gamma", "vol_skull_zeta", "vol_cell_b_island_hornet", "begin_zone_set:set_beach:backtrack", "begin_zone_set:set_beach:02:*", "vol_hornet_coop_test01", "vol_ringex01_elevator_start", "vol_hornet_coop_test02", "vol_ring03_halfway", "vol_cell_c_ridge_hornet", "vol_hornet_coop_test03", "vol_ridge_ground", "vol_ring_truth01", "vol_ring_truth02", "vol_lock_c_coop_teleport", "vol_cell_c_brute_spawn02", "vol_som"
		}
	},

	{LevelID::_map_id_halo3_cortana, std::vector<std::string>
		{
			"begin_zone_set:set_pelican_hill", "zone_set:set_pelican_hill", "zone_set:set_pelican_hill", "zone_set:set_pelican_hill", "begin_zone_set:set_bridge", "zone_set:set_bridge", "begin_zone_set:set_prisoner", "zone_set:set_prisoner", "begin_zone_set:set_reactor", "begin_zone_set:set_reactor", "zone_set:set_reactor", "begin_zone_set:set_bridge_return", "begin_zone_set:set_bridge_return", "zone_set:set_bridge_return", "begin_zone_set:set_pelican_hill_return", "zone_set:set_pelican_hill_return", "kill_01", "kill_02", "kill_reactor_goo", "vol_hall1_burst", "tv_enc_pelican_hill", "tv_ph_01", "tv_ph_02", "tv_ph_03", "tv_ph_04", "tv_ph_05", "tv_ph_06", "tv_ph_07", "tv_ph_08", "tv_ph_09", "tv_enc_halls_a_b", "tv_hab_01", "tv_hab_02", "tv_hab_03", "tv_hab_04", "tv_hab_05", "tv_hab_06", "tv_enc_bridge", "tv_br_02", "tv_br_03", "tv_br_04", "vol_cafe_below", "tv_enc_hall_c", "tv_hc_01", "tv_hc_02", "tv_hc_03", "tv_hc_04", "tv_hc_05", "tv_enc_prisoner", "tv_pr_01", "tv_pr_02", "tv_pr_03", "tv_enc_hall_d", "vol_110ce_start", "vol_110ce", "tv_enc_reactor", "vol_see_pylon_fall", "vol_pylon_failsafe_01", "vol_pylon_failsafe_02", "tv_re_01", "tv_re_02", "tv_re_03", "tv_re_04", "tv_re_05", "tv_re_06", "vol_reactor_goo", "vol_last_gasp", "tv_enc_inner_sanctum", "vol_inner_sanctum", "vol_rescue_return", "tv_enc_reactor_return", "vol_reactor_rev_center", "vol_pylon_01", "vol_pylon_02", "vol_pylon_03", "vol_pylon_04", "vol_reactor_way_out", "vol_flood_blockage", "tv_enc_hall_c_return", "tv_enc_bridge_return", "tv_br_return_02", "tv_br_return_03", "tv_br_return_04", "tv_enc_halls_a_b_return", "vol_hallway_2_rev_start", "tv_enc_pelican_hill_return", "vol_pel_hill_rev_01", "vol_pel_rev_right", "vol_pel_rev_left", "vol_pel_hill_rev_top", "vol_pel_hill_rev_board", "vol_near_end_pelican", "vol_see_end_pelican", "vol_inside_pelican", "*****clear*****", "vol_intro_clear", "vol_pel_hill_clear", "vol_halls_2_3_clear", "vol_bridge_clear", "vol_hall_4_clear", "vol_prisoner_blockage_clear", "vol_prisoner_clear", "vol_hall_5_clear", "vol_reactor_clear", "vol_hall_6_clear", "vol_cortana_clear", "vol_hall6_rev_clear", "vol_reactor_rev_clear", "vol_hall4_rev_clear", "vol_bridge_rev_clear", "vol_halls23_rev_clear", "*****garbage*****", "tv_rec_ih_02", "vol_garbage_pel_hill", "vol_garbage_hallway_2_3", "vol_garbage_bridge", "vol_garbage_hallway_4", "vol_garbage_prisoner", "vol_garbage_hallway_5", "vol_garbage_reactor", "*****temp*****", "vol_hall23_corty_temp", "vol_110cd", "vol_110cb_01", "vol_110cb_02", "vol_110cb_03", "vol_110cb_04", "vol_110cb_05", "vol_zone_set_ab", "vol_zone_set_bc", "vol_zone_set_cd", "vol_zone_set_de", "vol_zone_set_bc_end", "vol_zone_set_cd_too", "tv_fict_01_grave", "tv_fict_02_cortana", "tv_fict_03_pel_radio", "tv_fict_04_grave", "tv_fict_05_grave", "tv_fict_06_cortana", "tv_fict_08_cortana", "tv_fict_09_grave", "tv_fict_10_cortana", "tv_fict_11_grave", "tv_fict_12_14_grave", "tv_fict_14_continue", "tv_ih_01a", "tv_ih_02", "tv_ih_03", "tv_ih_04", "tv_ih_05", "tv_rec_ih_01", "tv_ih_01b", "tv_nav_exit_01", "tv_nav_exit_02", "tv_nav_exit_03", "tv_nav_exit_04", "tv_nav_exit_05", "tv_nav_exit_06", "tv_nav_exit_07", "tv_nav_exit_08", "tv_nav_exit_pelican", "vol_see_pylon_fall_rev", "tv_rec_reactor_bot", "tv_fict_13_cortana", "tv_br_01", "begin_zone_set:set_pelican_hill", "tv_br_05", "tv_hc_06", "tv_hd_01", "tv_pr_04", "tv_fict_17_cortana", "tv_fict_16_cortana", "tv_is_01", "tv_music_12_13", "tv_is_destroy_objects", "vol_pelican_ledge", "tv_br_return_01"
		}
	},

	{LevelID::_map_id_halo3_halo, std::vector<std::string>
		{
			"120_03_valley_floor_start", "begin_zone_set:120_01_02_03", "zone_set:120_04_100_110:*", "begin_zone_set:120_04_100_110:*", "teleport_players_trig", "third_right_trig", "third_left_trig", "third_center_trig", "second_left_trig", "second_center_trig", "second_right_trig", "ziggurat_final_trig", "johnson_intro_trig", "120_04_ziggurat_start", "third_right_pure_trig", "second_left_pure_trig", "120_08_trench_run_start", "120_07_path_to_exit_start", "valley_volume01", "valley_volume02", "valley_volume03", "valley_volume04", "120_07_hall_explosion02", "120_07_hall_explosion09", "120_07_hall_explosion03", "120_07_hall_explosion04", "120_07_hall_explosion01", "120_07_hall_explosion05", "120_07_cliff_section_start", "120_07_cliff_section02", "120_07_cliff_section03", "120_07_c_hw_section_start", "120_07_cliff_section01", "120_07_c_hw_section_02", "120_07_c_hw_section_05", "120_05_ziggurat_top_trig", "pureform_teleport_trig", "120_02_drop_down_start", "zone_set:120_110_120", "cor_dialog_trig01", "cor_dialog_trig02", "cor_dialog_trig03", "trench_run_100_trig00", "120_08_trench_encounter_100", "120_07_arbiter_revive_trig", "zone_set:120_07_06_04_03", "cor_dialog_trig02_5", "120_03_flood_attack", "120_05_arbiter_enters", "120_07_c_hw_section_04", "trench_run_game_save01", "trench_run_game_save02", "trench_run_game_save03", "trench_run_game_save04", "trench_run_100_trig01", "trench_run_100_trig02", "trench_run_100_trig03", "trench_run_100_trig04", "trench_run_100_trig06", "trench_run_100_trig07", "trench_run_100_trig08", "trench_run_100_island_trig01", "trench_run_110_trig01", "trench_run_110_trig02", "trench_run_110_trig03", "trench_run_110_trig04", "trench_run_110_trig05", "trench_run_110_trig06", "trench_run_110_trig07", "trench_run_110_trig08", "trench_run_110_trig09", "trench_run_110_trig10", "trench_run_110_trig11", "trench_run_110_trig12", "120_08_trench_encounter_120", "120_08_trench_encounter_110", "trench_run_110_island_trig01", "garbage_100_trig", "garbage_110_trig", "garbage_120_trig", "trench_run_110_trig13", "trench_run_110_trig14", "trench_run_110_trig15", "trench_run_120_trig01", "trench_run_120_trig02", "trench_run_120_trig03", "trench_run_120_trig04", "trench_run_120_trig05", "trench_run_120_trig07", "trench_run_120_trig06", "trench_run_120_trig08", "trench_run_120_trig09", "trench_run_120_trig10", "trench_run_120_trig14", "trench_run_120_trig15", "trench_run_120_trig16", "trench_run_120_trig17", "trench_run_120_trig18", "trench_run_120_trig19", "trench_run_120_trig20", "120_04_ziggurat_start02", "valley_floor_trig01", "valley_floor_trig02", "valley_floor_trig04", "valley_floor_trig05", "valley_floor_trig06", "valley_floor_trig03", "johnson_intro_trig02", "garbage_060_trig", "garbage_060_trig01", "120_07_outside_trig", "end_trilogy01", "trench_run_120_trig11", "trench_run_120_trig12", "garbage_060_trig02", "end_trilogy02", "end_trilogy03", "kill", "kill", "ziggurat_kill_player01", "ziggurat_kill_player02", "kill", "kill", "kill", "kill", "120_07_hall_explosion06", "120_07_hall_explosion07", "120_07_hall_explosion08", "120_07_hall_explosion10", "120_07_hall_explosion11", "120_07_hall_explosion12", "ai_vol100_01", "trench_run_110_trig09x", "trench_run_110_island_trig02", "ai_vol100_02", "trench_run_100_trig00", "trench_run_120_trig13", "trench_run_110_trig09x1", "ai_vol110_01", "ai_vol110_02", "ai_vol110_03", "ai_vol110_04", "ai_vol110_05", "ai_vol110_06", "110_island_01_exit", "trench_run_120_trig04x", "trench_run_120_trig12x", "ai_vol110_07", "tunnel01_trig01", "tunnel01_trig02", "tunnel01_trig03", "tunnel01_trig04", "tunnel01_trig05", "tunnel01_trig06", "tunnel01_trig07", "tunnel02_trig01", "tunnel02_trig02", "tunnel02_trig03", "tunnel02_trig04", "tunnel02_trig05", "tunnel02_trig06", "tunnel02_trig07", "tunnel02_trig08", "ai_vol110_08", "ai_vol110_09", "ai_vol110_10", "ai_vol110_11", "ai_vol110_12", "120_07_c_hall01", "120_07_c_hall02", "120_07_c_hall03", "120_07_c_hall04", "120_07_c_hall05", "valley_floor_trig00", "valley_floor_trig07", "zone_set:120_00_01_02", "hall_tj_memories_trig01", "hall_tj_memories_trig00", "begin_zone_set:120_100_110:*", "zone_set:120_04_100_110:*", "begin_zone_set:120_110_120:*", "zone_set:120_100_110:*", "zone_set:120_120_130:*", "zone_set:120_110_120:*", "begin_zone_set:120_120_130:*", "100_garbage", "kill", "trench_run_plate_100_hw_trig", "trench_run_plate_110_2_trig", "trench_run_plate_110_1_trig", "trench_run_plate_100_hw_unload_trig", "trench_run_plate_110_3_trig", "trench_run_plate_100_trig", "trench_run_plate_110_4_trig", "trench_run_plate_110_hw_trig", "kill", "kill", "trench_run_plate_120_1_trig", "trench_run_plate_120_2_trig", "trench_run_plate_120_3_trig", "trench_run_plate_120_4_trig", "100_game_save_trig", "110_game_save_trig", "110_game_save02_trig", "begin_zone_set:120_03_05", "zone_set:120_01_02_03", "begin_zone_set:120_07_06_04_03", "control_room_trig", "cor_dialog_trig01a", "cor_dialog_trig01b", "cor_dialog_trig01c", "cor_dialog_trig02a", "cor_dialog_trig02b", "cor_dialog_trig04", "cor_dialog_trig05", "valley_floor_cortana_dialog01", "zone_set:120_03_05", "100_island_trig02", "third_corner_trig", "ziggurat_start_trig", "100_island_trig01", "top_ziggurat_stairs", "100_island_trig00", "110_island02_trig_exit", "ai_vol110_13", "110_island_trig02", "island_110_02_trig01", "island_110_02_trig00", "island_110_02_trig02", "kill_100_01", "kill_100_02", "kill_100_03", "kill_100_04", "kill_110_01", "kill_110_02", "kill_110_03", "kill_110_04", "kill_110_05", "kill_120_01", "kill_120_02", "kill_120_03", "kill_120_04", "kill_120_05", "kill_100_02a", "110_island_trig00", "kill_120_04a", "kill_110_01a", "kill_pure_forms_trig", "kill_final_pure_forms_trig", "120_07_c_hall_end", "kill", "tv_boss_past_johnson", "tv_grunty_love", "120_07_c_gravemind_03"
		}
	},

	{LevelID::_map_id_halo3odst_mombasa_streets, std::vector<std::string>
		{

		}
	},

	{LevelID::_map_id_halo3odst_tayari_plaza, std::vector<std::string>
		{

		}
	},

	{LevelID::_map_id_halo3odst_uplift_reserve, std::vector<std::string>
		{

		}
	},

	{LevelID::_map_id_halo3odst_kizingo_boulevard, std::vector<std::string>
		{

		}
	},

	{LevelID::_map_id_halo3odst_oni_alpha_site, std::vector<std::string>
		{

		}
	},

	{LevelID::_map_id_halo3odst_nmpd_hq, std::vector<std::string>
		{

		}
	},

	{LevelID::_map_id_halo3odst_kikowani_station, std::vector<std::string>
		{

		}
	},

	{LevelID::_map_id_halo3odst_data_hive, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_halo3odst_coastal_highway, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_haloreach_noble_actual, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_haloreach_winter_contingency, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_haloreach_oni_sword_base, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_haloreach_nightfall, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_haloreach_tip_of_the_spear, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_haloreach_long_night_of_solace, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_haloreach_exodus, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_haloreach_new_alexandria, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_haloreach_the_package, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_haloreach_the_pillar_of_autumn, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_haloreach_lone_wolf, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_halo4_dawn, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_halo4_requiem, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_halo4_forerunner, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_halo4_infinity, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_halo4_reclaimer, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_halo4_shutdown, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_halo4_composer, std::vector<std::string>
		{

		}
	},

	{ LevelID::_map_id_halo4_midnight, std::vector<std::string>
		{

		}
	},

};



static inline void assertHardcodedTriggerCounts()
{
	assert(triggerNamesHardcoded.at(LevelID::_map_id_halo3_sierra_117).size() == 163);
	assert(triggerNamesHardcoded.at(LevelID::_map_id_halo3_crows_nest).size() == 237);
	assert(triggerNamesHardcoded.at(LevelID::_map_id_halo3_tsavo_highway).size() == 127);
	assert(triggerNamesHardcoded.at(LevelID::_map_id_halo3_the_storm).size() == 170);
	assert(triggerNamesHardcoded.at(LevelID::_map_id_halo3_floodgate).size() == 121);
	assert(triggerNamesHardcoded.at(LevelID::_map_id_halo3_the_ark).size() == 334);
	assert(triggerNamesHardcoded.at(LevelID::_map_id_halo3_the_covenant).size() == 253);
	assert(triggerNamesHardcoded.at(LevelID::_map_id_halo3_cortana).size() == 178);
	assert(triggerNamesHardcoded.at(LevelID::_map_id_halo3_halo).size() == 254);
}