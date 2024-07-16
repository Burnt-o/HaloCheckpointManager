**Halo 3 Script Commands:**  
Please note that an entry being present here doesn't necessarily mean that it'll work - it may do nothing, or maybe even crash the game.  

**TODO:** Remove items from this list that don't do anything or crash the game. Feel free to make a pull request!
```
; AVAILABLE FUNCTIONS:

(<passthrough> begin <expression(s)>)

(<passthrough> begin_random <expression(s)>)

(<passthrough> if <boolean> <then> [<else>])

(<passthrough> cond (<boolean1> <result1>) [(<boolean2> <result2>) [...]])

(<passthrough> set <variable name> <expression>)

(<boolean> and <boolean(s)>)

(<boolean> or <boolean(s)>)

(<real> + <number(s)>)

(<real> - <number> <number>)

(<real> * <number(s)>)

(<real> / <number> <number>)

(<real> min <number(s)>)

(<real> max <number(s)>)

(<boolean> = <expression> <expression>)

(<boolean> != <expression> <expression>)

(<boolean> > <number> <number>)

(<boolean> < <number> <number>)

(<boolean> >= <number> <number>)

(<boolean> <= <number> <number>)

(<void> sleep <short> [<script>])

(<void> sleep_for_ticks <short> [<script>])

(<void> sleep_forever [<script>])

(<boolean> sleep_until <boolean> [<short>])

(<void> wake <script name>)

(<void> inspect <expression>)

(<unit> unit <object>)

(<void> evaluate <script>)

(<boolean> not <boolean>)

(<real> pin <real> <real> <real>)

(<void> print <string>)

(<void> log_print <string>)

(<void> debug_scripting <boolean>)

(<void> debug_scripting_globals <boolean>)

(<void> debug_scripting_variable <string> <boolean>)

(<void> debug_scripting_variable_all <boolean>)

(<void> breakpoint <string>)

(<void> kill_active_scripts)

(<long> get_executing_running_thread)

(<void> kill_thread <long>)

(<boolean> script_started <string>)

(<boolean> script_finished <string>)

(<object_list> players)

(<unit> player_get <long>)

(<void> kill_volume_enable <trigger_volume>)

(<void> kill_volume_disable <trigger_volume>)

(<void> volume_teleport_players_not_inside <trigger_volume> <cutscene_flag>)

(<boolean> volume_test_object <trigger_volume> <object>)

(<boolean> volume_test_objects <trigger_volume> <object_list>)

(<boolean> volume_test_objects_all <trigger_volume> <object_list>)

(<boolean> volume_test_players <trigger_volume>)

(<boolean> volume_test_players_all <trigger_volume>)

(<object_list> volume_return_objects <trigger_volume>)

(<object_list> volume_return_objects_by_type <trigger_volume> <long>)

(<void> zone_set_trigger_volume_enable <string_id> <boolean>)

(<object> list_get <object_list> <short>)

(<short> list_count <object_list>)

(<short> list_count_not_dead <object_list>)

(<void> effect_new <effect> <cutscene_flag>)

(<void> effect_new_random <effect> <point_reference>)

(<void> effect_new_at_ai_point <effect> <point_reference>)

(<void> effect_new_on_object_marker <effect> <object> <string_id>)

(<void> effect_new_on_ground <effect> <object>)

(<void> damage_new <damage> <cutscene_flag>)

(<void> damage_object_effect <damage> <object>)

(<void> damage_objects_effect <damage> <object_list>)

(<void> damage_object <object> <string_id> <real>)

(<void> damage_objects <object_list> <string_id> <real>)

(<void> damage_players <damage>)

(<void> soft_ceiling_enable <string_id> <boolean>)

(<void> object_create <object_name>)

(<void> object_create_clone <object_name>)

(<void> object_create_anew <object_name>)

(<void> object_create_if_necessary <object_name>)

(<void> object_create_containing <string>)

(<void> object_create_clone_containing <string>)

(<void> object_create_anew_containing <string>)

(<void> object_create_folder <folder>)

(<void> object_create_folder_anew <folder>)

(<void> object_destroy <object>)

(<void> object_destroy_containing <string>)

(<void> object_destroy_all)

(<void> object_destroy_type_mask <long>)

(<void> objects_delete_by_definition <object_definition>)

(<void> object_destroy_folder <folder>)

(<void> object_hide <object> <boolean>)

(<void> object_set_shadowless <object> <boolean>)

(<real> object_buckling_magnitude_get <object>)

(<void> object_function_set <long> <real>)

(<void> object_set_function_variable <object> <string_id> <real> <real>)

(<void> object_clear_function_variable <object> <string_id>)

(<void> object_clear_all_function_variables <object>)

(<void> object_dynamic_simulation_disable <object> <boolean>)

(<void> object_set_phantom_power <object> <boolean>)

(<void> object_wake_physics <object>)

(<void> object_set_ranged_attack_inhibited <object> <boolean>)

(<void> object_set_melee_attack_inhibited <object> <boolean>)

(<real> object_get_health <object>)

(<real> object_get_shield <object>)

(<void> object_set_shield_effect <object> <real> <real>)

(<void> object_set_physics <object> <boolean>)

(<object> object_get_parent <object>)

(<void> objects_attach <object> <string_id> <object> <string_id>)

(<object> object_at_marker <object> <string_id>)

(<void> objects_detach <object> <object>)

(<void> object_set_scale <object> <real> <short>)

(<void> object_set_velocity <object> <real>)

(<void> object_set_inertia_tensor_scale <object> <real>)

(<void> object_set_collision_damage_armor_scale <object> <real>)

(<void> object_set_velocity <object> <real> <real> <real>)

(<void> object_set_deleted_when_deactivated <object>)

(<void> object_copy_player_appearance <object> <long>)

(<boolean> object_model_target_destroyed <object> <string_id>)

(<short> object_model_targets_destroyed <object> <string_id>)

(<void> object_damage_damage_section <object> <string_id> <real>)

(<boolean> object_vitality_pinned <object>)

(<void> garbage_collect_now)

(<void> garbage_collect_unsafe)

(<void> garbage_collect_multiplayer)

(<void> object_cannot_take_damage <object_list>)

(<void> object_can_take_damage <object_list>)

(<void> object_cinematic_lod <object> <boolean>)

(<void> object_cinematic_collision <object> <boolean>)

(<void> object_cinematic_visibility <object> <boolean>)

(<void> object_teleport <object> <cutscene_flag>)

(<void> object_teleport_to_ai_point <object> <point_reference>)

(<void> object_set_facing <object> <cutscene_flag>)

(<void> object_set_shield <object> <real>)

(<void> object_set_shield_normalized <object> <real>)

(<void> object_set_shield_stun <object> <real>)

(<void> object_set_shield_stun_infinite <object>)

(<void> object_set_permutation <object> <string_id> <string_id>)

(<void> object_set_variant <object> <string_id>)

(<void> object_set_region_state <object> <string_id> <model_state>)

(<boolean> objects_can_see_object <object_list> <object> <real>)

(<boolean> objects_can_see_flag <object_list> <cutscene_flag> <real>)

(<real> objects_distance_to_object <object_list> <object>)

(<real> objects_distance_to_flag <object_list> <cutscene_flag>)

(<real> objects_distance_to_position <object_list> <real> <real> <real>)

(<object_list> position_return_nearby_objects_by_type <real> <real> <real> <real> <long>)

(<void> script_doc)

(<void> help <string>)

(<object_list> game_engine_objects)

(<short> random_range <short> <short>)

(<real> real_random_range <real> <real>)

(<void> physics_constants_reset)

(<void> physics_set_gravity <real>)

(<void> physics_set_velocity_frame <real> <real> <real>)

(<void> physics_disable_character_ground_adhesion_forces <real>)

(<void> breakable_surfaces_enable <boolean>)

(<void> breakable_surfaces_reset)

(<boolean> render_lights <boolean>)

(<void> render_lights_enable_cinematic_shadow <boolean> <object> <string_id> <real>)

(<void> texture_camera_set_object_marker <object> <string_id> <real>)

(<void> texture_camera_set_position <real> <real> <real>)

(<void> texture_camera_set_target <real> <real> <real>)

(<void> texture_camera_attach_to_object <object> <string_id>)

(<void> texture_camera_target_object <object> <string_id>)

(<void> texture_camera_position_world_offset <real> <real> <real>)

(<void> texture_camera_on)

(<void> texture_camera_bink)

(<void> texture_camera_off)

(<void> texture_camera_set_aspect_ratio <real>)

(<void> texture_camera_set_resolution <long> <long>)

(<void> texture_camera_render_mode <long>)

(<void> texture_camera_set_fov <real>)

(<void> texture_camera_set_fov_frame_target <real>)

(<void> texture_camera_enable_dynamic_lights <boolean>)

(<void> render_postprocess_color_tweaking_reset)

(<void> scenery_animation_start <scenery> <animation_graph> <string_id>)

(<void> scenery_animation_start_loop <scenery> <animation_graph> <string_id>)

(<void> scenery_animation_start_relative <scenery> <animation_graph> <string_id> <object>)

(<void> scenery_animation_start_relative_loop <scenery> <animation_graph> <string_id> <object>)

(<void> scenery_animation_start_at_frame <scenery> <animation_graph> <string_id> <short>)

(<void> scenery_animation_start_relative_at_frame <scenery> <animation_graph> <string_id> <object> <short>)

(<void> scenery_animation_idle <scenery>)

(<short> scenery_get_animation_time <scenery>)

(<void> unit_can_blink <unit> <boolean>)

(<void> unit_set_active_camo <unit> <boolean> <real>)

(<void> unit_open <unit>)

(<void> unit_close <unit>)

(<void> unit_kill <unit>)

(<void> unit_kill_silent <unit>)

(<boolean> unit_is_emitting <unit>)

(<short> unit_get_custom_animation_time <unit>)

(<void> unit_stop_custom_animation <unit>)

(<boolean> custom_animation <unit> <animation_graph> <string_id> <boolean>)

(<boolean> custom_animation_loop <unit> <animation_graph> <string_id> <boolean>)

(<boolean> custom_animation_relative <unit> <animation_graph> <string_id> <boolean> <object>)

(<boolean> custom_animation_relative_loop <unit> <animation_graph> <string_id> <boolean> <object>)

(<boolean> custom_animation_list <object_list> <animation_graph> <string_id> <boolean>)

(<boolean> unit_custom_animation_at_frame <unit> <animation_graph> <string_id> <boolean> <short>)

(<boolean> unit_custom_animation_relative_at_frame <unit> <animation_graph> <string_id> <string_id> <boolean> <short>)

(<boolean> unit_is_playing_custom_animation <unit>)

(<void> object_set_custom_animations_hold_on_last_frame <boolean>)

(<void> object_set_custom_animations_prevent_lipsync_head_movement <boolean>)

(<boolean> preferred_animation_list_add <object> <string_id>)

(<void> unit_set_actively_controlled <unit> <boolean>)

(<short> unit_get_team_index <unit>)

(<void> unit_aim_without_turning <unit> <boolean>)

(<void> unit_set_enterable_by_player <unit> <boolean>)

(<boolean> unit_get_enterable_by_player <unit>)

(<void> unit_only_takes_damage_from_players_team <unit> <boolean>)

(<void> unit_enter_vehicle <unit> <vehicle> <string_id>)

(<void> unit_falling_damage_disable <unit> <boolean>)

(<boolean> unit_in_vehicle_type_mask <unit> <long>)

(<short> object_get_turret_count <object>)

(<vehicle> object_get_turret <object> <short>)

(<void> unit_board_vehicle <unit> <string_id>)

(<void> unit_set_emotion <unit> <long> <real> <real>)

(<void> unit_set_emotion_by_name <unit> <string_id> <real> <real>)

(<void> unit_enable_eye_tracking <unit> <boolean>)

(<void> unit_set_integrated_flashlight <unit> <boolean>)

(<void> unit_set_voice <unit> <any tag>)

(<void> unit_enable_vision_mode <unit> <boolean>)

(<boolean> unit_in_vehicle <unit>)

(<boolean> vehicle_test_seat_list <vehicle> <string_id> <object_list>)

(<boolean> vehicle_test_seat <vehicle> <string_id> <unit>)

(<void> unit_set_prefer_tight_camera_track <unit> <boolean>)

(<void> unit_exit_vehicle <unit>)

(<void> unit_exit_vehicle <unit> <short>)

(<void> unit_set_maximum_vitality <unit> <real> <real>)

(<void> units_set_maximum_vitality <object_list> <real> <real>)

(<void> unit_set_current_vitality <unit> <real> <real>)

(<void> units_set_current_vitality <object_list> <real> <real>)

(<short> vehicle_load_magic <object> <unit_seat_mapping> <object_list>)

(<short> vehicle_unload <object> <unit_seat_mapping>)

(<void> unit_set_animation_mode <unit> <string_id>)

(<void> magic_melee_attack)

(<object_list> vehicle_riders <unit>)

(<unit> vehicle_driver <unit>)

(<unit> vehicle_gunner <unit>)

(<real> unit_get_health <unit>)

(<real> unit_get_shield <unit>)

(<short> unit_get_total_grenade_count <unit>)

(<boolean> unit_has_weapon <unit> <object_definition>)

(<boolean> unit_has_weapon_readied <unit> <object_definition>)

(<boolean> unit_has_any_equipment <unit>)

(<boolean> unit_has_equipment <unit> <object_definition>)

(<void> unit_lower_weapon <unit> <short>)

(<void> unit_raise_weapon <unit> <short>)

(<void> unit_drop_support_weapon <unit>)

(<void> unit_spew_action <unit>)

(<void> unit_force_reload <unit>)

(<void> unit_doesnt_drop_items <object_list>)

(<void> unit_impervious <object_list> <boolean>)

(<void> unit_suspended <unit> <boolean>)

(<void> unit_add_equipment <unit> <starting_profile> <boolean> <boolean>)

(<void> weapon_hold_trigger <weapon> <long> <boolean>)

(<void> weapon_enable_warthog_chaingun_light <boolean>)

(<void> device_set_never_appears_locked <device> <boolean>)

(<void> device_set_power <device> <real>)

(<real> device_get_power <device>)

(<boolean> device_set_position <device> <real>)

(<real> device_get_position <device>)

(<void> device_set_position_immediate <device> <real>)

(<real> device_group_get <device_group>)

(<boolean> device_group_set <device> <device_group> <real>)

(<void> device_group_set_immediate <device_group> <real>)

(<void> device_one_sided_set <device> <boolean>)

(<void> device_ignore_player_set <device> <boolean>)

(<void> device_operates_automatically_set <device> <boolean>)

(<void> device_closes_automatically_set <device> <boolean>)

(<void> device_group_change_only_once_more_set <device_group> <boolean>)

(<boolean> device_set_position_track <device> <string_id> <real>)

(<boolean> device_set_overlay_track <device> <string_id>)

(<void> device_animate_position <device> <real> <real> <real> <real> <boolean>)

(<void> device_animate_overlay <device> <real> <real> <real> <real>)

(<void> ai_enable <boolean>)

(<boolean> ai_enabled)

(<void> ai_grenades <boolean>)

(<void> ai_dialogue_enable <boolean>)

(<void> ai_infection_suppress <long>)

(<object> ai_get_object <ai>)

(<unit> ai_get_unit <ai>)

(<ai> ai_get_squad <ai>)

(<ai> ai_get_turret_ai <ai> <short>)

(<point_reference> ai_random_smart_point <point_reference> <real> <real> <real>)

(<point_reference> ai_nearest_point <object> <point_reference>)

(<long> ai_get_point_count <point_reference>)

(<point_reference> ai_point_set_get_point <point_reference> <short>)

(<void> ai_place <ai>)

(<void> ai_place <ai> <short>)

(<void> ai_place_in_vehicle <ai> <ai>)

(<void> ai_cannot_die <ai> <boolean>)

(<boolean> ai_vitality_pinned <ai>)

(<void> ai_resurrect <object>)

(<void> ai_kill <ai>)

(<void> ai_kill_silent <ai>)

(<void> ai_erase <ai>)

(<void> ai_erase_all)

(<void> ai_disposable <ai> <boolean>)

(<void> ai_select <ai>)

(<void> ai_set_deaf <ai> <boolean>)

(<void> ai_set_blind <ai> <boolean>)

(<void> ai_set_weapon_up <ai> <boolean>)

(<void> ai_flood_disperse <ai> <ai>)

(<void> ai_magically_see <ai> <ai>)

(<void> ai_magically_see_object <ai> <object>)

(<void> ai_set_active_camo <ai> <boolean>)

(<void> ai_suppress_combat <ai> <boolean>)

(<void> ai_migrate <ai> <ai>)

(<void> ai_migrate_persistent <ai> <ai>)

(<void> ai_allegiance <team> <team>)

(<void> ai_allegiance_remove <team> <team>)

(<void> ai_allegiance_break <team> <team>)

(<void> ai_braindead <ai> <boolean>)

(<void> ai_braindead_by_unit <object_list> <boolean>)

(<void> ai_disregard <object_list> <boolean>)

(<void> ai_prefer_target <object_list> <boolean>)

(<void> ai_prefer_target_team <ai> <team>)

(<void> ai_prefer_target_ai <ai> <ai> <boolean>)

(<void> ai_set_targeting_group <ai> <short>)

(<void> ai_set_targeting_group <ai> <short> <boolean>)

(<void> ai_teleport_to_starting_location_if_outside_bsp <ai>)

(<void> ai_teleport <ai> <point_reference>)

(<void> ai_bring_forward <object> <real>)

(<short> ai_migrate_form <ai> <ai> <short>)

(<boolean> ai_morph <ai> <short>)

(<boolean> biped_morph <object>)

(<void> ai_renew <ai>)

(<void> ai_force_active <ai> <boolean>)

(<void> ai_force_active_by_unit <unit> <boolean>)

(<void> ai_playfight <ai> <boolean>)

(<void> ai_berserk <ai> <boolean>)

(<void> ai_set_team <ai> <team>)

(<void> ai_allow_dormant <ai> <boolean>)

(<boolean> ai_is_attacking <ai>)

(<short> ai_fighting_count <ai>)

(<short> ai_living_count <ai>)

(<real> ai_living_fraction <ai>)

(<short> ai_in_vehicle_count <ai>)

(<short> ai_body_count <ai>)

(<real> ai_strength <ai>)

(<short> ai_swarm_count <ai>)

(<short> ai_nonswarm_count <ai>)

(<object_list> ai_actors <ai>)

(<boolean> ai_allegiance_broken <team> <team>)

(<short> ai_spawn_count <ai>)

(<ai> object_get_ai <object>)

(<boolean> ai_rotate_scenario <real> <real> <real> <real>)

(<boolean> ai_translate_scenario <real> <real> <real>)

(<void> scenario_duplicate_bsp_objects <string> <string>)

(<void> scenario_duplicate_bsp_objects <string> <string> <string>)

(<void> scenario_rotate <real> <real> <real> <real>)

(<void> scenario_reflect <real> <real> <real> <real>)

(<void> scenario_translate <real> <real> <real>)

(<void> scenario_rotate <real> <real> <real> <real> <string> <string>)

(<void> scenario_reflect <real> <real> <real> <real> <string> <string>)

(<void> scenario_translate <real> <real> <real> <string> <string>)

(<boolean> ai_set_task <ai> <string_id> <string_id>)

(<boolean> ai_set_objective <ai> <string_id>)

(<short> ai_task_status <ai>)

(<boolean> ai_set_task_condition <ai> <boolean>)

(<boolean> ai_leadership <ai>)

(<boolean> ai_leadership_all <ai>)

(<short> ai_task_count <ai>)

(<void> ai_activity_set <ai> <string_id>)

(<void> ai_activity_abort <ai>)

(<vehicle> ai_vehicle_get <ai>)

(<vehicle> ai_vehicle_get_from_starting_location <ai>)

(<short> ai_vehicle_get_squad_count <ai>)

(<vehicle> ai_vehicle_get_from_squad <ai> <short>)

(<boolean> ai_vehicle_reserve_seat <vehicle> <unit_seat_mapping> <boolean>)

(<boolean> ai_vehicle_reserve <vehicle> <boolean>)

(<ai> ai_player_get_vehicle_squad <unit>)

(<short> ai_vehicle_count <ai>)

(<boolean> ai_carrying_player <ai>)

(<boolean> player_in_vehicle <vehicle>)

(<boolean> ai_player_needs_vehicle <unit>)

(<boolean> ai_player_any_needs_vehicle)

(<void> ai_vehicle_enter <ai> <unit> <unit_seat_mapping>)

(<void> ai_vehicle_enter <ai> <unit>)

(<void> ai_vehicle_enter_immediate <ai> <unit> <unit_seat_mapping>)

(<void> ai_vehicle_enter_immediate <ai> <unit>)

(<short> ai_enter_squad_vehicles <ai>)

(<void> ai_vehicle_exit <ai> <unit_seat_mapping>)

(<void> ai_vehicle_exit <ai>)

(<boolean> vehicle_overturned <vehicle>)

(<void> vehicle_flip <vehicle>)

(<short> ai_combat_status <ai>)

(<boolean> flock_start <string_id>)

(<boolean> flock_stop <string_id>)

(<boolean> flock_create <string_id>)

(<boolean> flock_delete <string_id>)

(<boolean> flock_definition_set <string_id> <any tag>)

(<void> flock_unperch <string_id>)

(<boolean> ai_wall_lean <ai>)

(<real> ai_play_line <ai> <ai_line>)

(<real> ai_play_line_at_player <ai> <ai_line>)

(<real> ai_play_line_on_object <object> <ai_line>)

(<real> ai_play_line_on_object_for_team <object> <ai_line> <mp_team>)

(<short> ai_play_line_on_point_set <string_id> <point_reference> <short>)

(<short> ai_play_line_on_point_set <string_id> <point_reference> <short> <string_id>)

(<void> campaign_metagame_time_pause <boolean>)

(<void> campaign_metagame_award_points <object> <short>)

(<void> campaign_metagame_award_primary_skull <object> <short>)

(<void> campaign_metagame_award_secondary_skull <object> <short>)

(<boolean> campaign_metagame_enabled)

(<boolean> campaign_is_finished_easy)

(<boolean> campaign_is_finished_normal)

(<boolean> campaign_is_finished_heroic)

(<boolean> campaign_is_finished_legendary)

(<void> cs_run_command_script <ai> <ai_command_script>)

(<void> cs_queue_command_script <ai> <ai_command_script>)

(<void> cs_stack_command_script <ai> <ai_command_script>)

(<void> vs_reserve <ai> <short>)

(<void> vs_reserve <ai> <boolean> <short>)

(<boolean> vs_cast <ai> <boolean> <short> <string_id>)

(<boolean> vs_cast <ai> <boolean> <short> <string_id> <string_id>)

(<boolean> vs_cast <ai> <boolean> <short> <string_id> <string_id> <string_id>)

(<boolean> vs_cast <ai> <boolean> <short> <string_id> <string_id> <string_id> <string_id>)

(<boolean> vs_cast <ai> <boolean> <short> <string_id> <string_id> <string_id> <string_id> <string_id>)

(<boolean> vs_cast <ai> <boolean> <short> <string_id> <string_id> <string_id> <string_id> <string_id> <string_id>)

(<boolean> vs_cast <ai> <boolean> <short> <string_id> <string_id> <string_id> <string_id> <string_id> <string_id> <string_id>)

(<ai> vs_role <short>)

(<void> vs_abort_on_alert <boolean>)

(<void> vs_abort_on_damage <boolean>)

(<void> vs_abort_on_combat_status <short>)

(<void> vs_abort_on_vehicle_exit <boolean>)

(<void> cs_abort_on_alert <boolean>)

(<void> cs_abort_on_damage <boolean>)

(<void> cs_abort_on_combat_status <short>)

(<void> cs_abort_on_vehicle_exit <boolean>)

(<void> cs_abort_on_alert <boolean>)

(<void> vs_abort_on_alert <ai> <boolean>)

(<void> cs_abort_on_damage <boolean>)

(<void> vs_abort_on_damage <ai> <boolean>)

(<void> cs_abort_on_combat_status <short>)

(<void> vs_abort_on_combat_status <ai> <short>)

(<void> cs_abort_on_vehicle_exit <boolean>)

(<void> vs_abort_on_vehicle_exit <ai> <boolean>)

(<void> vs_set_cleanup_script <script>)

(<void> vs_release <ai>)

(<void> vs_release_all)

(<boolean> cs_command_script_running <ai> <ai_command_script>)

(<boolean> cs_command_script_queued <ai> <ai_command_script>)

(<short> cs_number_queued <ai>)

(<boolean> cs_moving)

(<boolean> cs_moving <ai>)

(<boolean> vs_running_atom <ai>)

(<boolean> vs_running_atom_movement <ai>)

(<boolean> vs_running_atom_action <ai>)

(<boolean> vs_running_atom_dialogue <ai>)

(<void> cs_fly_to <point_reference>)

(<void> vs_fly_to <ai> <boolean> <point_reference>)

(<void> cs_fly_to <point_reference> <real>)

(<void> vs_fly_to <ai> <boolean> <point_reference> <real>)

(<void> cs_fly_to_and_face <point_reference> <point_reference>)

(<void> vs_fly_to_and_face <ai> <boolean> <point_reference> <point_reference>)

(<void> cs_fly_to_and_face <point_reference> <point_reference> <real>)

(<void> vs_fly_to_and_face <ai> <boolean> <point_reference> <point_reference> <real>)

(<void> cs_fly_by <point_reference>)

(<void> vs_fly_by <ai> <boolean> <point_reference>)

(<void> cs_fly_by <point_reference> <real>)

(<void> vs_fly_by <ai> <boolean> <point_reference> <real>)

(<void> cs_go_to <point_reference>)

(<void> vs_go_to <ai> <boolean> <point_reference>)

(<void> cs_go_to <point_reference> <real>)

(<void> vs_go_to <ai> <boolean> <point_reference> <real>)

(<void> cs_go_by <point_reference> <point_reference>)

(<void> vs_go_by <ai> <boolean> <point_reference> <point_reference>)

(<void> cs_go_by <point_reference> <point_reference> <real>)

(<void> vs_go_by <ai> <boolean> <point_reference> <point_reference> <real>)

(<void> cs_go_to_and_face <point_reference> <point_reference>)

(<void> vs_go_to_and_face <ai> <boolean> <point_reference> <point_reference>)

(<void> cs_go_to_and_posture <point_reference> <string_id>)

(<void> vs_go_to_and_posture <ai> <boolean> <point_reference> <string_id>)

(<void> cs_go_to_nearest <point_reference>)

(<void> vs_go_to_nearest <ai> <boolean> <point_reference>)

(<void> cs_move_in_direction <real> <real> <real>)

(<void> vs_move_in_direction <ai> <boolean> <real> <real> <real>)

(<void> cs_move_towards <object>)

(<void> vs_move_towards <ai> <boolean> <object>)

(<void> cs_move_towards <object> <boolean>)

(<void> vs_move_towards <ai> <boolean> <object> <boolean>)

(<void> cs_swarm_to <point_reference> <real>)

(<void> vs_swarm_to <ai> <boolean> <point_reference> <real>)

(<void> cs_swarm_from <point_reference> <real>)

(<void> vs_swarm_from <ai> <boolean> <point_reference> <real>)

(<void> cs_pause <real>)

(<void> vs_pause <ai> <boolean> <real>)

(<void> cs_grenade <point_reference> <short>)

(<void> vs_grenade <ai> <boolean> <point_reference> <short>)

(<void> cs_equipment <point_reference>)

(<void> vs_equipment <ai> <boolean> <point_reference>)

(<void> cs_jump <real> <real>)

(<void> vs_jump <ai> <boolean> <real> <real>)

(<void> cs_jump_to_point <real> <real>)

(<void> vs_jump_to_point <ai> <boolean> <real> <real>)

(<void> cs_vocalize <short>)

(<void> vs_vocalize <ai> <boolean> <short>)

(<void> cs_play_sound <sound>)

(<void> vs_play_sound <ai> <boolean> <sound>)

(<void> cs_play_sound <sound> <real>)

(<void> vs_play_sound <ai> <boolean> <sound> <real>)

(<void> cs_play_sound <sound> <real> <real>)

(<void> vs_play_sound <ai> <boolean> <sound> <real> <real>)

(<void> cs_action <point_reference> <short>)

(<void> vs_action <ai> <boolean> <point_reference> <short>)

(<void> cs_action_at_object <object> <short>)

(<void> vs_action_at_object <ai> <boolean> <object> <short>)

(<void> cs_action_at_player <short>)

(<void> vs_action_at_player <ai> <boolean> <short>)

(<void> cs_custom_animation <animation_graph> <string_id> <boolean>)

(<void> vs_custom_animation <ai> <boolean> <animation_graph> <string_id> <boolean>)

(<void> cs_custom_animation <animation_graph> <string_id> <boolean> <point_reference>)

(<void> vs_custom_animation <ai> <boolean> <animation_graph> <string_id> <boolean> <point_reference>)

(<void> cs_custom_animation_death <animation_graph> <string_id> <boolean>)

(<void> vs_custom_animation_death <ai> <boolean> <animation_graph> <string_id> <boolean>)

(<void> cs_custom_animation_death <animation_graph> <string_id> <boolean> <point_reference>)

(<void> vs_custom_animation_death <ai> <boolean> <animation_graph> <string_id> <boolean> <point_reference>)

(<void> cs_custom_animation_loop <animation_graph> <string_id> <boolean>)

(<void> vs_custom_animation_loop <ai> <animation_graph> <string_id> <boolean>)

(<void> cs_custom_animation_loop <animation_graph> <string_id> <boolean> <point_reference>)

(<void> vs_custom_animation_loop <ai> <animation_graph> <string_id> <boolean> <point_reference>)

(<void> cs_play_line <ai_line>)

(<void> vs_play_line <ai> <boolean> <ai_line>)

(<void> cs_die <short>)

(<void> vs_die <ai> <boolean> <short>)

(<void> cs_deploy_turret <point_reference>)

(<void> vs_deploy_turret <ai> <boolean> <point_reference>)

(<void> cs_approach <object> <real> <real> <real>)

(<void> vs_approach <ai> <boolean> <object> <real> <real> <real>)

(<void> cs_approach_player <real> <real> <real>)

(<void> vs_approach_player <ai> <boolean> <real> <real> <real>)

(<void> cs_go_to_vehicle <vehicle>)

(<void> vs_go_to_vehicle <ai> <boolean> <vehicle>)

(<void> cs_go_to_vehicle <vehicle> <unit_seat_mapping>)

(<void> vs_go_to_vehicle <ai> <boolean> <vehicle> <unit_seat_mapping>)

(<void> cs_set_style <style>)

(<void> vs_set_style <ai> <style>)

(<void> cs_force_combat_status <short>)

(<void> vs_force_combat_status <ai> <short>)

(<void> cs_enable_targeting <boolean>)

(<void> vs_enable_targeting <ai> <boolean>)

(<void> cs_enable_looking <boolean>)

(<void> vs_enable_looking <ai> <boolean>)

(<void> cs_enable_moving <boolean>)

(<void> vs_enable_moving <ai> <boolean>)

(<void> cs_enable_dialogue <boolean>)

(<void> vs_enable_dialogue <ai> <boolean>)

(<void> cs_suppress_activity_termination <boolean>)

(<void> vs_suppress_activity_termination <ai> <boolean>)

(<void> cs_suppress_dialogue_global <boolean>)

(<void> vs_suppress_dialogue_global <ai> <boolean>)

(<void> cs_look <boolean> <point_reference>)

(<void> vs_look <ai> <boolean> <point_reference>)

(<void> cs_look_player <boolean>)

(<void> vs_look_player <ai> <boolean>)

(<void> cs_look_object <boolean> <object>)

(<void> vs_look_object <ai> <boolean> <object>)

(<void> cs_aim <boolean> <point_reference>)

(<void> vs_aim <ai> <boolean> <point_reference>)

(<void> cs_aim_player <boolean>)

(<void> vs_aim_player <ai> <boolean>)

(<void> cs_aim_object <boolean> <object>)

(<void> vs_aim_object <ai> <boolean> <object>)

(<void> cs_face <boolean> <point_reference>)

(<void> vs_face <ai> <boolean> <point_reference>)

(<void> cs_face_player <boolean>)

(<void> vs_face_player <ai> <boolean>)

(<void> cs_face_object <boolean> <object>)

(<void> vs_face_object <ai> <boolean> <object>)

(<void> cs_shoot <boolean>)

(<void> vs_shoot <ai> <boolean>)

(<void> cs_shoot <boolean> <object>)

(<void> vs_shoot <ai> <boolean> <object>)

(<void> cs_shoot_point <boolean> <point_reference>)

(<void> vs_shoot_point <ai> <boolean> <point_reference>)

(<void> cs_shoot_secondary_trigger <boolean>)

(<void> vs_shoot_secondary_trigger <ai> <boolean>)

(<void> cs_lower_weapon <boolean>)

(<void> vs_lower_weapon <ai> <boolean>)

(<void> cs_vehicle_speed <real>)

(<void> vs_vehicle_speed <ai> <real>)

(<void> cs_vehicle_speed_instantaneous <real>)

(<void> vs_vehicle_speed_instantaneous <ai> <real>)

(<void> cs_vehicle_boost <boolean>)

(<void> vs_vehicle_boost <ai> <boolean>)

(<void> cs_turn_sharpness <boolean> <real>)

(<void> vs_turn_sharpness <ai> <boolean> <real>)

(<void> cs_enable_pathfinding_failsafe <boolean>)

(<void> vs_enable_pathfinding_failsafe <ai> <boolean>)

(<void> cs_set_pathfinding_radius <real>)

(<void> vs_set_pathfinding_radius <ai> <real>)

(<void> cs_ignore_obstacles <boolean>)

(<void> vs_ignore_obstacles <ai> <boolean>)

(<void> cs_approach_stop)

(<void> vs_approach_stop <ai>)

(<void> cs_movement_mode <short>)

(<void> vs_movement_mode <ai> <short>)

(<void> cs_crouch <boolean>)

(<void> vs_crouch <ai> <boolean>)

(<void> cs_crouch <boolean> <real>)

(<void> vs_crouch <ai> <boolean> <real>)

(<void> cs_walk <boolean>)

(<void> vs_walk <ai> <boolean>)

(<void> cs_posture_set <string_id> <boolean>)

(<void> vs_posture_set <ai> <string_id> <boolean>)

(<void> cs_posture_exit)

(<void> vs_posture_exit <ai>)

(<void> cs_stow)

(<void> vs_stow <ai>)

(<void> cs_draw)

(<void> vs_draw <ai>)

(<void> cs_teleport <point_reference> <point_reference>)

(<void> vs_teleport <ai> <point_reference> <point_reference>)

(<void> cs_stop_custom_animation)

(<void> vs_stop_custom_animation <ai>)

(<void> cs_stop_sound <sound>)

(<void> vs_stop_sound <ai> <sound>)

(<void> cs_player_melee <long>)

(<void> vs_player_melee <ai> <boolean> <long>)

(<void> cs_melee_direction <long>)

(<void> vs_melee_direction <ai> <boolean> <long>)

(<void> cs_smash_direction <long>)

(<void> vs_smash_direction <ai> <boolean> <long>)

(<void> camera_control <boolean>)

(<void> camera_set <cutscene_camera_point> <short>)

(<void> camera_set_relative <cutscene_camera_point> <short> <object>)

(<void> camera_set_animation <animation_graph> <string_id>)

(<void> camera_set_animation_relative <animation_graph> <string_id> <unit> <cutscene_flag>)

(<void> camera_set_animation_with_speed <animation_graph> <string_id> <real>)

(<void> camera_set_animation_relative_with_speed <animation_graph> <string_id> <unit> <cutscene_flag> <real>)

(<void> camera_set_animation_relative_with_speed_loop <animation_graph> <string_id> <unit> <cutscene_flag> <real> <boolean>)

(<void> camera_set_animation_relative_with_speed_loop_offset <animation_graph> <string_id> <unit> <cutscene_flag> <real> <boolean> <real>)

(<void> camera_predict_resources_at_frame <animation_graph> <string_id> <unit> <cutscene_flag> <long>)

(<void> camera_predict_resources_at_point <cutscene_camera_point>)

(<void> camera_set_first_person <unit>)

(<void> camera_set_cinematic)

(<void> camera_set_cinematic_scene <cinematic scene definition> <long> <cutscene_flag>)

(<short> camera_time)

(<void> camera_set_field_of_view <real> <short>)

(<void> cinematic_camera_set_easing_in <real> <real> <real> <real> <real> <real>)

(<void> cinematic_camera_set_easing_out <real>)

(<void> cinematic_print <string>)

(<void> camera_set_pan <cutscene_camera_point> <short>)

(<void> camera_pan <cutscene_camera_point> <cutscene_camera_point> <short> <short> <real> <short> <real>)

(<game_difficulty> game_difficulty_get)

(<game_difficulty> game_difficulty_get_real)

(<short> game_insertion_point_get)

(<void> pvs_set_object <object>)

(<void> pvs_set_camera <cutscene_camera_point>)

(<void> pvs_clear)

(<void> players_unzoom_all)

(<void> player_enable_input <boolean>)

(<void> player_disable_movement <boolean>)

(<void> player_disable_weapon_pickup <boolean>)

(<boolean> player_flashlight_on)

(<boolean> player_active_camouflage_on)

(<boolean> player_camera_control <boolean>)

(<void> player_action_test_reset)

(<boolean> player_action_test_jump)

(<boolean> player_action_test_primary_trigger)

(<boolean> player_action_test_grenade_trigger)

(<boolean> player_action_test_vision_trigger)

(<boolean> player_action_test_zoom)

(<boolean> player_action_test_rotate_weapons)

(<boolean> player_action_test_rotate_grenades)

(<boolean> player_action_test_melee)

(<boolean> player_action_test_action)

(<boolean> player_action_test_accept)

(<boolean> player_action_test_cancel)

(<boolean> player_action_test_look_relative_up)

(<boolean> player_action_test_look_relative_down)

(<boolean> player_action_test_look_relative_left)

(<boolean> player_action_test_look_relative_right)

(<boolean> player_action_test_look_relative_all_directions)

(<boolean> player_action_test_move_relative_all_directions)

(<boolean> player_action_test_cinematic_skip)

(<boolean> player_action_test_start)

(<boolean> player_action_test_back)

(<boolean> player0_looking_up)

(<boolean> player0_looking_down)

(<void> player0_set_pitch <real> <long>)

(<void> player1_set_pitch <real> <long>)

(<void> player2_set_pitch <real> <long>)

(<void> player3_set_pitch <real> <long>)

(<void> player_action_test_look_up_begin <real> <real>)

(<void> player_action_test_look_down_begin <real> <real>)

(<void> player_action_test_look_pitch_end)

(<boolean> player_action_test_lookstick_forward)

(<boolean> player_action_test_lookstick_backward)

(<void> map_reset)

(<void> switch_zone_set <zone_set>)

(<long> current_zone_set)

(<long> current_zone_set_fully_active)

(<void> game_rate <real> <real> <real>)

(<void> fade_in <real> <real> <real> <short>)

(<void> fade_out <real> <real> <real> <short>)

(<void> cinematic_start)

(<void> cinematic_stop)

(<void> cinematic_skip_start_internal)

(<void> cinematic_skip_stop_internal)

(<void> cinematic_show_letterbox <boolean>)

(<void> cinematic_show_letterbox_immediate <boolean>)

(<void> cinematic_set_title <cutscene_title>)

(<void> cinematic_set_title_delayed <cutscene_title> <real>)

(<void> cinematic_suppress_bsp_object_creation <boolean>)

(<void> cinematic_subtitle <string_id> <real>)

(<void> cinematic_set <cinematic definition>)

(<void> cinematic_set_shot <cinematic scene definition> <long>)

(<void> cinematic_set_early_exit <long>)

(<long> cinematic_get_early_exit)

(<void> cinematic_set_active_camera <string_id>)

(<void> cinematic_object_create <string>)

(<void> cinematic_object_create_cinematic_anchor <string> <cutscene_flag>)

(<void> cinematic_object_destroy <string>)

(<void> cinematic_destroy)

(<void> cinematic_clips_initialize_for_shot <long>)

(<void> cinematic_clips_destroy)

(<void> cinematic_lights_initialize_for_shot <long>)

(<void> cinematic_lights_destroy)

(<void> cinematic_light_object <object> <string_id> <cinematic_lightprobe> <cutscene_camera_point>)

(<void> cinematic_light_object_off <object>)

(<void> cinematic_lighting_rebuild_all)

(<object> cinematic_object_get <string>)

(<unit> cinematic_object_get_unit <string>)

(<scenery> cinematic_object_get_scenery <string>)

(<effect_scenery> cinematic_object_get_effect_scenery <string>)

(<void> camera_set_briefing <boolean>)

(<cinematic definition> cinematic_tag_reference_get_cinematic <long>)

(<cinematic scene definition> cinematic_tag_reference_get_scene <long> <long>)

(<effect> cinematic_tag_reference_get_effect <long> <long> <long> <long>)

(<sound> cinematic_tag_reference_get_dialogue <long> <long> <long> <long>)

(<sound> cinematic_tag_reference_get_music <long> <long> <long> <long>)

(<looping_sound> cinematic_tag_reference_get_music_looping <long> <long> <long> <long>)

(<animation_graph> cinematic_tag_reference_get_animation <long> <long> <long>)

(<boolean> cinematic_scripting_object_coop_flags_valid <long> <long> <long>)

(<void> cinematic_scripting_fade_out <real> <real> <real> <short>)

(<void> cinematic_scripting_create_object <long> <long> <long> <object_name>)

(<void> cinematic_scripting_create_cinematic_object <long> <long> <long>)

(<void> cinematic_scripting_start_animation <long> <long> <long> <object> <string_id>)

(<void> cinematic_scripting_destroy_object <long> <long> <long> <object>)

(<void> cinematic_scripting_start_effect <long> <long> <long> <long> <object>)

(<void> cinematic_scripting_start_music <long> <long> <long> <long>)

(<void> cinematic_scripting_start_dialogue <long> <long> <long> <long> <object>)

(<void> cinematic_scripting_stop_music <long> <long> <long> <long>)

(<void> cinematic_scripting_create_and_animate_object <long> <long> <long> <object_name> <object> <string_id> <boolean>)

(<void> cinematic_scripting_create_and_animate_cinematic_object <long> <long> <long> <object> <string_id> <boolean>)

(<void> cinematic_scripting_create_and_animate_object_no_animation <long> <long> <long> <object_name> <object> <boolean>)

(<void> cinematic_scripting_create_and_animate_cinematic_object_no_animation <long> <long> <long> <object> <boolean>)

(<void> cinematic_scripting_play_cortana_effect <long> <long> <long> <long>)

(<void> game_won)

(<void> game_lost <boolean>)

(<void> game_revert)

(<boolean> game_is_cooperative)

(<boolean> game_is_playtest)

(<void> game_can_use_flashlights <boolean>)

(<void> game_insertion_point_unlock <short>)

(<void> game_insertion_point_lock <short>)

(<boolean> achievement_was_earned_by_player <short> <string_id>)

(<void> achievement_grant_to_player <short> <string_id>)

(<void> core_load)

(<void> core_load_name <string>)

(<void> core_save)

(<void> core_save_name <string>)

(<boolean> game_safe_to_save)

(<boolean> game_safe_to_speak)

(<boolean> game_all_quiet)

(<void> game_save)

(<void> game_save_cancel)

(<void> game_save_no_timeout)

(<void> game_save_immediate)

(<boolean> game_saving)

(<boolean> game_reverted)

(<void> sound_impulse_predict <sound>)

(<void> sound_impulse_trigger <sound> <object> <real> <long>)

(<void> sound_impulse_start <sound> <object> <real>)

(<void> sound_impulse_start_cinematic <sound> <object> <real> <real> <real>)

(<void> sound_impulse_start_effect <sound> <object> <real> <string_id>)

(<void> sound_impulse_start_with_subtitle <sound> <object> <real> <string_id> <string_id>)

(<long> sound_impulse_time <sound>)

(<long> sound_impulse_time <sound> <long>)

(<long> sound_impulse_language_time <sound>)

(<void> sound_impulse_stop <sound>)

(<void> sound_impulse_start_3d <sound> <real> <real>)

(<void> sound_impulse_mark_as_outro <sound>)

(<void> sound_looping_predict <looping_sound>)

(<void> sound_looping_start <looping_sound> <object> <real>)

(<void> sound_looping_stop <looping_sound>)

(<void> sound_looping_stop_immediately <looping_sound>)

(<void> sound_looping_set_scale <looping_sound> <real>)

(<void> sound_looping_set_alternate <looping_sound> <boolean>)

(<void> sound_class_set_gain <string> <real> <short>)

(<void> sound_class_set_gain_db <string> <real> <short>)

(<void> sound_class_enable_ducker <string> <boolean>)

(<void> sound_set_global_effect <string_id> <real>)

(<void> sound_set_global_effect_scale <string_id> <real>)

(<void> vehicle_auto_turret <vehicle> <trigger_volume> <real> <real> <real>)

(<void> vehicle_hover <vehicle> <boolean>)

(<long> vehicle_count_bipeds_killed <vehicle>)

(<void> biped_ragdoll <unit>)

(<void> water_float_reset <object>)

(<void> hud_show_training_text <boolean>)

(<void> hud_set_training_text <string_id>)

(<void> hud_enable_training <boolean>)

(<void> player_training_activate_flashlight)

(<void> player_training_activate_crouch)

(<void> player_training_activate_stealth)

(<void> player_training_activate_jump)

(<void> hud_activate_team_nav_point_flag <team> <cutscene_flag> <real>)

(<void> hud_deactivate_team_nav_point_flag <team> <cutscene_flag>)

(<void> hud_breadcrumbs_activate_team_nav_point_flag <team> <cutscene_flag> <real>)

(<void> hud_breadcrumbs_deactivate_team_nav_point_flag <team> <cutscene_flag>)

(<void> hud_breadcrumbs_activate_team_nav_point_position <team> <real> <real> <real> <string> <real>)

(<void> hud_breadcrumbs_deactivate_team_nav_point_position <team> <string>)

(<void> hud_breadcrumbs_activate_team_nav_point_object <team> <object> <string> <real>)

(<void> hud_breadcrumbs_deactivate_team_nav_point_object <team> <string>)

(<boolean> hud_breadcrumbs_using_revised_nav_points)

(<object> object_get_variant_child_object_by_marker_id <object> <string_id>)

(<void> chud_texture_cam <boolean>)

(<void> play_cortana_effect <string_id>)

(<void> chud_show <boolean>)

(<void> chud_show_weapon_stats <boolean>)

(<void> chud_show_crosshair <boolean>)

(<void> chud_show_shield <boolean>)

(<void> chud_show_grenades <boolean>)

(<void> chud_show_messages <boolean>)

(<void> chud_show_motion_sensor <boolean>)

(<void> chud_show_spike_grenades <boolean>)

(<void> chud_show_fire_grenades <boolean>)

(<void> chud_cinematic_fade <real> <real>)

(<void> player_effect_set_max_translation <real> <real> <real>)

(<void> player_effect_set_max_rotation <real> <real> <real>)

(<void> player_effect_set_max_rumble <real> <real>)

(<void> player_effect_start <real> <real>)

(<void> player_effect_stop <real>)

(<void> render_atmosphere_fog <boolean>)

(<void> motion_blur <boolean>)

(<void> render_weather <boolean>)

(<void> render_patchy_fog <boolean>)

(<void> controller_set_look_invert <boolean>)

(<boolean> controller_get_look_invert)

(<void> xoverlapped_debug_render <boolean>)

(<void> cc_enable <boolean>)

(<void> objectives_clear)

(<void> objectives_show_up_to <long>)

(<void> objectives_finish_up_to <long>)

(<void> objectives_secondary_show <long>)

(<void> objectives_secondary_finish <long>)

(<void> objectives_secondary_unavailable <long>)

(<void> input_suppress_rumble <boolean>)

(<void> play_bink_movie <string>)

(<void> play_bink_movie_from_tag <bink definition>)

(<long> bink_time)

(<void> set_global_doppler_factor <real>)

(<void> set_global_mixbin_headroom <long> <long>)

(<void> data_mine_set_mission_segment <string>)

(<void> data_mine_insert <string>)

(<void> data_mine_enable <boolean>)

(<void> data_mine_debug_menu_setting <string>)

(<object_list> object_list_children <object> <object_definition>)

(<long> interpolator_start <string_id> <real> <real>)

(<long> interpolator_start_smooth <string_id> <real> <real>)

(<long> interpolator_stop <string_id>)

(<long> interpolator_restart <string_id>)

(<boolean> interpolator_is_active <string_id>)

(<boolean> interpolator_is_finished <string_id>)

(<long> interpolator_set_current_value <string_id> <real>)

(<real> interpolator_get_current_value <string_id> <boolean>)

(<real> interpolator_get_start_value <string_id> <boolean>)

(<real> interpolator_get_final_value <string_id> <boolean>)

(<real> interpolator_get_current_phase <string_id>)

(<real> interpolator_get_current_time_fraction <string_id>)

(<real> interpolator_get_start_time <string_id>)

(<real> interpolator_get_final_time <string_id>)

(<real> interpolator_evaluate_at <string_id> <real> <boolean>)

(<real> interpolator_evaluate_at_time_fraction <string_id> <real> <boolean>)

(<real> interpolator_evaluate_at_time <string_id> <real> <boolean>)

(<real> interpolator_evaluate_at_time_delta <string_id> <real> <boolean>)

(<void> interpolator_stop_all)

(<void> interpolator_restart_all)

(<void> interpolator_flip)

(<void> cinematic_clone_players_weapon <object> <string_id> <string_id>)

(<void> cinematic_move_attached_objects <object> <string_id> <string_id> <string_id>)

(<void> vehicle_enable_ghost_effects <boolean>)

(<void> set_global_sound_environment <real> <real> <real> <real> <long> <real>)

(<void> game_save_cinematic_skip)

(<void> cinematic_outro_start)

(<void> cinematic_enable_ambience_details <boolean>)

(<void> cache_block_for_one_frame)

(<void> sound_suppress_ambience_update_on_revert)

(<void> render_autoexposure_enable <boolean>)

(<void> render_exposure_full <real> <real> <real> <real> <real>)

(<void> render_exposure_fade_in <real> <real>)

(<void> render_exposure_fade_out <real>)

(<void> render_exposure <real> <real>)

(<void> render_autoexposure_instant <long>)

(<void> render_exposure_set_environment_darken <real>)

(<void> render_depth_of_field_enable <boolean>)

(<void> render_depth_of_field <real> <real> <real> <real>)

(<void> render_dof_focus_depth <real>)

(<void> render_dof_blur_animate <real> <real>)

(<object_list> mp_players_by_team <mp_team>)

(<long> mp_active_player_count_by_team <mp_team>)

(<void> deterministic_end_game)

(<void> mp_game_won <mp_team>)

(<void> mp_respawn_override_timers <mp_team>)

(<void> mp_ai_allegiance <team> <mp_team>)

(<void> mp_allegiance <mp_team> <mp_team>)

(<boolean> mp_round_started)

(<void> give_medal <short>)

(<void> mp_scripts_reset)

(<void> mp_ai_place <ai>)

(<void> mp_ai_place <ai> <short>)

(<void> mp_ai_kill <ai>)

(<void> mp_ai_kill_silent <ai>)

(<void> mp_object_create <object_name>)

(<void> mp_object_create_clone <object_name>)

(<void> mp_object_create_anew <object_name>)

(<void> mp_object_destroy <object>)

(<void> mp_object_belongs_to_team <object> <mp_team>)

(<void> mp_weapon_belongs_to_team <object> <mp_team>)

(<void> set_camera_third_person <long> <boolean>)

(<boolean> get_camera_third_person <long>)

(<boolean> mover_set_program <object> <short>)

(<void> predict_bink_movie <string>)

(<void> predict_bink_movie_from_tag <bink definition>)

(<void> camera_set_mode <long> <long>)

(<void> camera_set_flying_cam_at_point <long> <cutscene_camera_point>)

(<long> game_coop_player_count)

(<void> add_recycling_volume <trigger_volume> <long> <long>)

(<long> game_tick_get)

(<void> saved_film_set_playback_game_speed <real>)

(<void> saved_film_set_pending_playback_game_speed <real>)

(<void> designer_zone_activate <designer_zone>)

(<void> designer_zone_deactivate <designer_zone>)

(<void> object_set_always_active <object> <boolean>)

(<boolean> tag_is_active <any tag not resolving>)

(<void> object_set_persistent <object> <boolean>)

(<void> sound_looping_start_with_effect <looping_sound> <object> <real> <string_id>)

(<void> set_performance_throttle <string> <long> <real>)

(<real> get_performance_throttle <string> <long>)

(<void> cinematic_zone_activate <long>)

(<void> cinematic_zone_deactivate <long>)

(<long> tiling_current)

(<void> unit_limit_lipsync_to_mouth_only <unit> <boolean>)

(<boolean> unit_start_first_person_custom_animation <unit> <animation_graph> <string_id> <boolean>)

(<boolean> unit_is_playing_custom_first_person_animation <unit>)

(<void> unit_stop_first_person_custom_animation <unit>)

(<void> prepare_to_switch_to_zone_set <zone_set>)

(<void> cinematic_zone_activate_for_debugging <long>)

(<void> unit_play_random_ping <unit>)

(<void> player_control_fade_out_all_input <real>)

(<void> player_control_fade_in_all_input <real>)

(<void> player_control_lock_gaze <unit> <point_reference> <real>)

(<void> player_control_unlock_gaze <unit>)

(<void> player_control_scale_all_input <real> <real>)

(<bink definition> cinematic_tag_reference_get_bink <long>)

(<void> object_set_custom_animation_speed <object> <real>)

(<void> scenery_animation_start_at_frame_loop <scenery> <animation_graph> <string_id> <short>)

(<void> saved_film_set_repro_mode <boolean>)

(<void> chud_show_arbiter_ai_navpoint <boolean>)

(<cinematic scene definition> cortana_tag_reference_get_scene <long>)

(<void> font_set_emergency)

(<void> biped_force_ground_fitting_on <unit> <boolean>)

(<void> cinematic_set_chud_objective <cutscene_title>)

(<boolean> terminal_is_being_read)

(<boolean> terminal_was_accessed <object>)

(<boolean> terminal_was_completed <object>)

(<void> cortana_terminal_was_accessed)

(<weapon> unit_get_primary_weapon <unit>)

(<animation_graph> budget_resource_get_animation_graph <animation budget reference>)

(<void> game_award_level_complete_achievements)

(<looping_sound> budget_resource_get_looping_sound <looping sound budget reference>)

(<void> game_safe_to_respawn <boolean>)

(<void> cortana_effect_kill)

(<void> cinematic_scripting_destroy_cortana_effect_cinematic)

(<void> ai_migrate_infanty <ai> <ai>)

(<void> render_cinematic_motion_blur <boolean>)

(<void> ai_dont_do_avoidance <ai> <boolean>)

(<void> cinematic_scripting_clean_up <long>)

(<void> ai_erase_inactive <ai> <short>)

(<void> stop_bink_movie)

(<void> play_credits_unskippable)

(<sound> budget_resource_get_sound <sound budget reference>)

(<boolean> is_ace_build)

(<void> ssao_enable <short>)

(<void> ssao_radius <real>)

(<void> ssao_intensity <real>)

(<void> ssao_sampleZThreshold <real>)

(<void> color_grading_enable <boolean>)

(<void> lightshafts_enable <boolean>)

(<long> simulation_profiler_detail_level <long>)

(<boolean> simulation_profiler_enable_downstream_processing <boolean>)

(<void> mp_wake_script <string>)

; AVAILABLE EXTERNAL GLOBALS:

(<boolean> debug_controller_latency)

(<real> camera_fov_scale)

(<boolean> effects_corpse_nonviolent)

(<real> sound_global_room_gain)

(<boolean> enable_pc_sound)

(<boolean> debug_duckers)

(<boolean> debug_sound_listeners)

(<boolean> use_dynamic_object_obstruction)

(<boolean> disable_expensive_physics_rebuilding)

(<real> minimum_havok_object_acceleration)

(<real> minimum_havok_biped_object_acceleration)

(<boolean> render_lightmap_shadows)

(<boolean> render_screen_flashes)

(<real> texture_camera_near_plane)

(<real> texture_camera_exposure)

(<real> texture_camera_illum_scale)

(<real> render_near_clip_distance)

(<real> render_far_clip_distance)

(<real> render_HDR_target_stops)

(<long> render_surface_LDR_mode)

(<long> render_surface_HDR_mode)

(<real> render_first_person_fov_scale)

(<real> render_postprocess_hue)

(<real> render_postprocess_saturation)

(<real> render_postprocess_red_filter)

(<real> render_postprocess_green_filter)

(<real> render_postprocess_blue_filter)

(<real> render_bounce_light_intensity)

(<boolean> render_bounce_light_only)

(<long> render_debug_depth_render)

(<real> render_debug_depth_render_scale_r)

(<real> render_debug_depth_render_scale_g)

(<real> render_debug_depth_render_scale_b)

(<boolean> decal_disable)

(<real> water_physics_velocity_minimum)

(<real> water_physics_velocity_maximum)

(<boolean> breakable_surfaces)

(<ai> ai_current_squad)

(<ai> ai_current_actor)

(<short> ai_combat_status_asleep)

(<short> ai_combat_status_idle)

(<short> ai_combat_status_alert)

(<short> ai_combat_status_active)

(<short> ai_combat_status_uninspected)

(<short> ai_combat_status_definite)

(<short> ai_combat_status_certain)

(<short> ai_combat_status_visible)

(<short> ai_combat_status_clear_los)

(<short> ai_combat_status_dangerous)

(<short> ai_task_status_never)

(<short> ai_task_status_occupied)

(<short> ai_task_status_empty)

(<short> ai_task_status_inactive)

(<short> ai_task_status_exhausted)

(<short> ai_evaluator_preference)

(<short> ai_evaluator_avoidance)

(<short> ai_evaluator_sum)

(<short> ai_evaluator_pathfinding)

(<short> ai_evaluator_preferred_group)

(<short> ai_evaluator_pursuit_walkdistance)

(<short> ai_evaluator_pursuit_targetdistance)

(<short> ai_evaluator_pursuit_targethint)

(<short> ai_evaluator_pursuit_visible)

(<short> ai_evaluator_pursuit_examined_us)

(<short> ai_evaluator_pursuit_examined_total)

(<short> ai_evaluator_pursuit_available)

(<short> ai_evaluator_panic_walkdistance)

(<short> ai_evaluator_panic_targetdistance)

(<short> ai_evaluator_panic_closetotarget)

(<short> ai_evaluator_guard_walkdistance)

(<short> ai_evaluator_attack_weaponrange)

(<short> ai_evaluator_attack_idealrange)

(<short> ai_evaluator_attack_visible)

(<short> ai_evaluator_attack_dangerousenemy)

(<short> ai_evaluator_combatmove_walkdistance)

(<short> ai_evaluator_combatmove_lineoffire)

(<short> ai_evaluator_hide_cover)

(<short> ai_evaluator_hide_exposed)

(<short> ai_evaluator_uncover_pre_evaluate)

(<short> ai_evaluator_uncover_visible)

(<short> ai_evaluator_uncover_blocked)

(<short> ai_evaluator_previously_discarded)

(<short> ai_evaluator_danger_zone)

(<short> ai_evaluator_move_into_danger_zone)

(<short> ai_evaluator_3d_path_available)

(<short> ai_evaluator_point_avoidance)

(<short> ai_evaluator_point_preference)

(<short> ai_evaluator_directional_driving)

(<short> ai_evaluator_favor_former_firing_position)

(<short> ai_evaluator_hide_pre_evaluation)

(<short> ai_evaluator_pursuit)

(<short> ai_evaluator_pursuit_area_discarded)

(<short> ai_evaluator_flag_preferences)

(<short> ai_evaluator_perch_preferences)

(<short> ai_evaluator_combatmove_lineoffire_occluded)

(<short> ai_evaluator_attack_same_frame_of_reference)

(<short> ai_evaluator_wall_leanable)

(<short> ai_evaluator_cover_near_friends)

(<short> ai_evaluator_combat_move_near_follow_unit)

(<short> ai_evaluator_goal_preferences)

(<short> ai_evaluator_hint_plane)

(<short> ai_evaluator_postsearch_prefer_original)

(<short> ai_evaluator_leadership)

(<short> ai_evaluator_flee_to_leader)

(<short> ai_evaluator_goal_points_only)

(<short> ai_evaluator_attack_leader_distance)

(<short> ai_evaluator_too_far_from_leader)

(<short> ai_evaluator_guard_preference)

(<short> ai_evaluator_guard_wall_preference)

(<short> ai_evaluator_obstacle)

(<short> ai_evaluator_facing)

(<short> ai_action_berserk)

(<short> ai_action_surprise_front)

(<short> ai_action_surprise_back)

(<short> ai_action_evade_left)

(<short> ai_action_evade_right)

(<short> ai_action_dive_forward)

(<short> ai_action_dive_back)

(<short> ai_action_dive_left)

(<short> ai_action_dive_right)

(<short> ai_action_advance)

(<short> ai_action_cheer)

(<short> ai_action_fallback)

(<short> ai_action_hold)

(<short> ai_action_point)

(<short> ai_action_posing)

(<short> ai_action_shakefist)

(<short> ai_action_signal_attack)

(<short> ai_action_signal_move)

(<short> ai_action_taunt)

(<short> ai_action_warn)

(<short> ai_action_wave)

(<short> ai_activity_none)

(<short> ai_activity_patrol)

(<short> ai_activity_stand)

(<short> ai_activity_crouch)

(<short> ai_activity_stand_drawn)

(<short> ai_activity_crouch_drawn)

(<short> ai_activity_corner)

(<short> ai_activity_corner_open)

(<short> ai_activity_bunker)

(<short> ai_activity_bunker_open)

(<short> ai_activity_combat)

(<short> ai_activity_backup)

(<short> ai_activity_guard)

(<short> ai_activity_guard_crouch)

(<short> ai_activity_guard_wall)

(<short> ai_activity_typing)

(<short> ai_activity_kneel)

(<short> ai_activity_gaze)

(<short> ai_activity_poke)

(<short> ai_activity_sniff)

(<short> ai_activity_track)

(<short> ai_activity_watch)

(<short> ai_activity_examine)

(<short> ai_activity_sleep)

(<short> ai_activity_at_ease)

(<short> ai_activity_cower)

(<short> ai_activity_tai_chi)

(<short> ai_activity_pee)

(<short> ai_activity_doze)

(<short> ai_activity_eat)

(<short> ai_activity_medic)

(<short> ai_activity_work)

(<short> ai_activity_cheering)

(<short> ai_activity_injured)

(<short> ai_activity_captured)

(<boolean> morph_disallowed)

(<real> morph_time_ranged_tank)

(<real> morph_time_ranged_stealth)

(<real> morph_time_tank_ranged)

(<real> morph_time_tank_stealth)

(<real> morph_time_stealth_ranged)

(<real> morph_time_stealth_tank)

(<short> morph_form_ranged)

(<short> morph_form_tank)

(<short> morph_form_stealth)

(<short> ai_movement_patrol)

(<short> ai_movement_asleep)

(<short> ai_movement_combat)

(<short> ai_movement_flee)

(<short> ai_movement_flaming)

(<short> ai_movement_stunned)

(<short> ai_movement_berserk)

(<boolean> enable_pc_joystick)

(<boolean> sapien_keyboard_toggle_for_camera_movement)

(<boolean> enable_console_window)

(<boolean> use_tool_command_legacy)

(<long> maximum_tool_command_history)

(<boolean> use_temp_directory_for_files)

(<boolean> enable_tag_resource_prediction)

(<boolean> enable_entire_pvs_prediction)

(<boolean> enable_cluster_objects_prediction)

(<long> instance_default_fade_start_pixels)

(<long> instance_default_fade_end_pixels)

(<boolean> ignore_predefined_performance_throttles)

(<boolean> enable_first_person_prediction)

(<boolean> enable_structure_prediction)

(<boolean> render_alpha_to_coverage)

(<boolean> enable_sound_transmission)

(<long> motion_blur_taps)

(<real> motion_blur_expected_dt)

(<real> motion_blur_max_x)

(<real> motion_blur_max_y)

(<real> motion_blur_scale_x)

(<real> motion_blur_scale_y)

(<real> motion_blur_center_falloff)

(<boolean> disable_render_state_cache_optimization)

(<boolean> enable_better_cpu_gpu_sync)

(<long> motion_blur_max_viewport_count)

(<boolean> cinematic_prediction_enable)

(<boolean> skies_delete_on_zone_set_switch_enable)

(<boolean> reduce_widescreen_fov_during_cinematics)

(<boolean> use_hdr_bloom_scaling)

(<real> hdr_bloom_intensity)

(<boolean> use_memory_mirror_events)

(<boolean> sleep_for_vblank)

(<boolean> decorator_res_scaled)

(<boolean> allow_480p_resolutions)

```
