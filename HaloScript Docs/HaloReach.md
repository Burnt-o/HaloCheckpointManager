**Halo Reach Script Commands:**  
Please note that an entry being present here doesn't necessarily mean that it'll work - it may do nothing, or maybe even crash the game.  

**TODO:** Remove items from this list that don't do anything or crash the game. Feel free to make a pull request!
```
; AVAILABLE FUNCTIONS:

(<passthrough> begin <expression(s)>)

(<passthrough> begin_random <expression(s)>)

(<passthrough> begin_count <expression(s)>)

(<passthrough> begin_random_count <expression(s)>)

(<passthrough> if <boolean> <then> [<else>])

(<passthrough> cond (<boolean1> <result1>) [(<boolean2> <result2>) [...]])

(<passthrough> set <variable name> <expression>)

(<boolean> and <boolean(s)>)

(<boolean> or <boolean(s)>)

(<real> + <number(s)>)

(<real> - <number> <number>)

(<real> * <number(s)>)

(<real> / <number> <number>)

(<real> % <number> <number>)

(<real> min <number(s)>)

(<real> max <number(s)>)

(<boolean> = <expression> <expression>)

(<boolean> != <expression> <expression>)

(<boolean> > <number> <number>)

(<boolean> < <number> <number>)

(<boolean> >= <number> <number>)

(<boolean> <= <number> <number>)

(<void> sleep <short> [<script>])

(<void> sleep_forever [<script>])

(<boolean> sleep_until <boolean> [<short>])

(<boolean> sleep_until_game_ticks <boolean> [<short>])

(<void> wake <script name>)

(<void> cinematic_sleep <short> [<script>])

(<void> inspect <expression>)

(<void> branch <condition> <script>)

(<unit> unit <object>)

(<vehicle> vehicle <object>)

(<weapon> weapon <object>)

(<device> device <object>)

(<scenery> scenery <object>)

(<effect_scenery> effect_scenery <object>)

(<void> evaluate <script>)

(<boolean> not <boolean>)

(<real> pin <real> <real> <real>)

(<void> print <string>)

(<void> print_if <boolean> <string>)

(<void> log_print <string>)

(<void> debug_scripting <boolean>)

(<void> debug_scripting_globals <boolean>)

(<void> debug_scripting_variable <string> <boolean>)

(<void> debug_scripting_variable_all <boolean>)

(<void> breakpoint <string>)

(<void> kill_active_scripts)

(<long> get_executing_running_thread)

(<void> kill_thread <long>)

(<boolean> script_started <string_id>)

(<boolean> script_finished <string_id>)

(<object_list> local_players)

(<object_list> players)

(<object_list> players_human)

(<object_list> players_elite)

(<unit> player_get <long>)

(<player> player_in_game_get <long>)

(<player> human_player_in_game_get <long>)

(<player> elite_player_in_game_get <long>)

(<boolean> player_is_in_game <player>)

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

(<object_list> volume_return_objects_by_campaign_type <trigger_volume> <long>)

(<void> zone_set_trigger_volume_enable <string_id> <boolean>)

(<object> list_get <object_list> <short>)

(<short> list_count <object_list>)

(<short> list_count_not_dead <object_list>)

(<void> effect_new <effect> <cutscene_flag>)

(<void> effect_new_random <effect> <point_reference>)

(<void> effect_new_random <effect> <point_reference> <real> <real>)

(<void> effect_new_at_ai_point <effect> <point_reference>)

(<void> effect_new_at_point_set_point <effect> <point_reference> <long>)

(<void> effect_new_on_object_marker <effect> <object> <string_id>)

(<void> effect_new_on_object_marker_loop <effect> <object> <string_id>)

(<void> effect_stop_object_marker <effect> <object> <string_id>)

(<void> effect_new_on_ground <effect> <object>)

(<void> damage_new <damage> <cutscene_flag>)

(<void> damage_object_effect <damage> <object>)

(<void> damage_objects_effect <damage> <object_list>)

(<void> damage_object <object> <string_id> <real>)

(<void> damage_objects <object_list> <string_id> <real>)

(<void> damage_players <damage>)

(<void> soft_ceiling_enable <string_id> <boolean>)

(<void> object_create <object_name>)

(<void> object_create_variant <object_name> <string_id>)

(<void> object_create_clone <object_name>)

(<void> object_create_anew <object_name>)

(<void> object_create_if_necessary <object_name>)

(<void> object_create_folder <folder>)

(<void> object_create_folder_anew <folder>)

(<void> object_destroy <object>)

(<void> object_destroy_all)

(<void> object_destroy_type_mask <long>)

(<void> objects_delete_by_definition <object_definition>)

(<void> object_destroy_folder <folder>)

(<void> object_hide <object> <boolean>)

(<void> object_set_shadowless <object> <boolean>)

(<void> object_set_allegiance <object> <team>)

(<real> object_buckling_magnitude_get <object>)

(<void> object_function_set <long> <real>)

(<void> object_set_function_variable <object> <string_id> <real> <real>)

(<void> object_set_cinematic_function_variable <object> <string_id> <real> <real>)

(<void> object_clear_cinematic_function_variable <object> <string_id>)

(<void> object_clear_all_cinematic_function_variables <object>)

(<void> object_dynamic_simulation_disable <object> <boolean>)

(<void> object_set_phantom_power <object> <boolean>)

(<void> object_wake_physics <object>)

(<boolean> object_get_at_rest <object>)

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

(<short> object_get_bsp <object>)

(<void> object_set_as_fireteam_target <object> <boolean>)

(<boolean> object_is_reserved <object>)

(<void> object_set_velocity <object> <real> <real> <real>)

(<void> object_set_deleted_when_deactivated <object>)

(<void> object_copy_player_appearance <object> <long>)

(<boolean> object_model_target_destroyed <object> <string_id>)

(<short> object_model_targets_destroyed <object> <string_id>)

(<void> object_enable_damage_section <object> <string_id> <real>)

(<void> object_disable_damage_section <object> <string_id>)

(<void> object_damage_damage_section <object> <string_id> <real>)

(<void> object_ignores_emp <object> <boolean>)

(<boolean> object_vitality_pinned <object>)

(<void> garbage_collect_now)

(<void> garbage_collect_unsafe)

(<void> garbage_collect_multiplayer)

(<void> object_cannot_take_damage <object_list>)

(<real> object_get_recent_body_damage <object>)

(<real> object_get_recent_shield_damage <object>)

(<void> object_can_take_damage <object_list>)

(<void> object_immune_to_friendly_damage <object_list> <boolean>)

(<void> object_cinematic_lod <object> <boolean>)

(<void> object_cinematic_collision <object> <boolean>)

(<void> object_cinematic_visibility <object> <boolean>)

(<void> objects_predict <object_list>)

(<void> objects_predict_high <object_list>)

(<void> objects_predict_low <object_list>)

(<void> object_type_predict_high <object_definition>)

(<void> object_type_predict_low <object_definition>)

(<void> object_type_predict <object_definition>)

(<void> object_teleport <object> <cutscene_flag>)

(<void> object_teleport_to_ai_point <object> <point_reference>)

(<void> object_set_facing <object> <cutscene_flag>)

(<void> object_set_shield <object> <real>)

(<void> object_set_shield_stun <object> <real>)

(<void> object_set_shield_stun_infinite <object>)

(<void> object_set_permutation <object> <string_id> <string_id>)

(<void> object_set_variant <object> <string_id>)

(<void> object_set_region_state <object> <string_id> <model_state>)

(<void> object_set_model_state_property <object> <long> <boolean>)

(<boolean> objects_can_see_object <object_list> <object> <real>)

(<boolean> objects_can_see_flag <object_list> <cutscene_flag> <real>)

(<real> objects_distance_to_object <object_list> <object>)

(<real> objects_distance_to_flag <object_list> <cutscene_flag>)

(<real> objects_distance_to_point <object_list> <point_reference>)

(<void> position_predict <real> <real> <real>)

(<void> shader_predict <shader>)

(<void> bitmap_predict <bitmap>)

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

(<boolean> recording_play <unit> <cutscene_recording>)

(<boolean> recording_play_and_delete <unit> <cutscene_recording>)

(<boolean> recording_play_and_hover <vehicle> <cutscene_recording>)

(<void> recording_kill <unit>)

(<short> recording_time <unit>)

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

(<void> hud_camera_on <long>)

(<void> hud_camera_off <long>)

(<void> hud_camera_set_position <long> <real> <real> <real>)

(<void> hud_camera_set_target <long> <real> <real> <real>)

(<void> hud_camera_attach_to_object <long> <object> <string_id>)

(<void> hud_camera_target_object <long> <object> <string_id>)

(<void> hud_camera_structure <long> <boolean>)

(<void> hud_camera_highlight_object <long> <object> <real> <real> <real> <real>)

(<void> hud_camera_clear_objects <long>)

(<void> hud_camera_spin_around <long> <object>)

(<void> hud_camera_from_player_view <long>)

(<void> hud_camera_window <long> <real> <real> <real> <real>)

(<void> weather_animate <string_id> <real> <real>)

(<void> weather_animate_force <string_id> <real> <real>)

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

(<boolean> unit_is_emp_stunned <unit>)

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

(<boolean> unit_has_mandibles_hidden <unit>)

(<boolean> sync_action_create <unit> <string_id>)

(<boolean> object_running_sync_action <object> <string_id>)

(<void> object_set_custom_animations_hold_on_last_frame <boolean>)

(<void> object_set_custom_animations_prevent_lipsync_head_movement <boolean>)

(<boolean> preferred_animation_list_add <object> <string_id>)

(<short> unit_get_team_index <unit>)

(<void> unit_aim_without_turning <unit> <boolean>)

(<void> unit_set_enterable_by_player <unit> <boolean>)

(<boolean> unit_get_enterable_by_player <unit>)

(<void> unit_only_takes_damage_from_players_team <unit> <boolean>)

(<void> unit_enter_vehicle <unit> <vehicle> <string_id>)

(<void> unit_enter_vehicle_immediate <unit> <vehicle> <string_id>)

(<void> unit_falling_damage_disable <unit> <boolean>)

(<boolean> unit_in_vehicle_type <unit> <long>)

(<short> object_get_turret_count <object>)

(<vehicle> object_get_turret <object> <short>)

(<void> unit_board_vehicle <unit> <string_id>)

(<void> unit_set_emotion <unit> <long> <real> <real>)

(<void> unit_set_emotion_by_name <unit> <string_id> <real> <real>)

(<void> unit_enable_eye_tracking <unit> <boolean>)

(<void> unit_set_integrated_flashlight <unit> <boolean>)

(<boolean> unit_in_vehicle <unit>)

(<vehicle> unit_get_vehicle <unit>)

(<void> vehicle_set_player_interaction <vehicle> <unit_seat_mapping> <boolean> <boolean>)

(<void> vehicle_set_unit_interaction <vehicle> <unit_seat_mapping> <boolean> <boolean>)

(<boolean> vehicle_test_seat_unit_list <vehicle> <unit_seat_mapping> <object_list>)

(<boolean> vehicle_test_seat_unit <vehicle> <unit_seat_mapping> <unit>)

(<boolean> vehicle_test_seat <vehicle> <unit_seat_mapping>)

(<void> unit_set_prefer_tight_camera_track <unit> <boolean>)

(<boolean> unit_test_seat <unit> <unit_seat_mapping>)

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

(<void> weapon_set_primary_barrel_firing <weapon> <boolean>)

(<void> weapon_hold_trigger <weapon> <long> <boolean>)

(<void> weapon_enable_warthog_chaingun_light <boolean>)

(<void> device_set_never_appears_locked <device> <boolean>)

(<void> device_set_power <device> <real>)

(<void> device_set_position_transition_time <device> <real>)

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

(<void> ai_dialogue_updating_enable <boolean>)

(<void> ai_player_dialogue_enable <boolean>)

(<void> ai_actor_dialogue_enable <ai> <boolean>)

(<void> ai_actor_dialogue_effect_enable <ai> <boolean>)

(<void> ai_infection_suppress <long>)

(<void> ai_fast_and_dumb <boolean>)

(<void> ai_lod_full_detail_actors <short>)

(<void> ai_force_full_lod <ai>)

(<void> ai_force_low_lod <ai>)

(<void> ai_clear_lod <ai>)

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

(<void> ai_place_in_limbo <ai>)

(<void> ai_place_in_limbo <ai> <short>)

(<void> ai_place_in_vehicle <ai> <ai>)

(<void> ai_cannot_die <ai> <boolean>)

(<boolean> ai_vitality_pinned <ai>)

(<void> ai_place_wave <long> <ai>)

(<void> ai_place_wave <long> <ai> <short>)

(<void> ai_place_wave_in_limbo <long> <ai>)

(<void> ai_place_wave_in_limbo <long> <ai> <short>)

(<void> ai_set_clump <ai> <short>)

(<void> ai_designer_clump_perception_range <real>)

(<void> ai_designer_clump_targeting_group <short>)

(<ai> ai_index_from_spawn_formation <ai> <string_id>)

(<void> ai_resurrect <object>)

(<void> ai_kill <ai>)

(<void> ai_kill_silent <ai>)

(<void> ai_kill_no_statistics <ai>)

(<void> ai_erase <ai>)

(<void> ai_erase_all)

(<void> ai_disposable <ai> <boolean>)

(<void> ai_select <ai>)

(<void> ai_set_deaf <ai> <boolean>)

(<void> ai_set_blind <ai> <boolean>)

(<void> ai_set_weapon_up <ai> <boolean>)

(<void> ai_flood_disperse <ai> <ai>)

(<void> ai_add_navpoint <ai> <boolean>)

(<void> ai_magically_see <ai> <ai>)

(<void> ai_magically_see_object <ai> <object>)

(<void> ai_set_active_camo <ai> <boolean>)

(<void> ai_suppress_combat <ai> <boolean>)

(<void> ai_engineer_explode <ai>)

(<void> ai_grunt_kamikaze <ai>)

(<void> ai_squad_enumerate_immigrants <ai> <boolean>)

(<void> ai_migrate <ai> <ai>)

(<void> ai_migrate_persistent <ai> <ai>)

(<void> ai_allegiance <team> <team>)

(<void> ai_allegiance_remove <team> <team>)

(<void> ai_allegiance_break <team> <team>)

(<void> ai_braindead <ai> <boolean>)

(<void> ai_braindead_by_unit <object_list> <boolean>)

(<void> ai_disregard <object_list> <boolean>)

(<void> ai_disregard_orphans <ai>)

(<void> ai_prefer_target <object_list> <boolean>)

(<void> ai_prefer_target_team <ai> <team>)

(<void> ai_prefer_target_ai <ai> <ai> <boolean>)

(<void> ai_set_targeting_group <ai> <short>)

(<void> ai_set_targeting_group <ai> <short> <boolean>)

(<void> ai_teleport_to_starting_location_if_outside_bsp <ai>)

(<void> ai_teleport_to_spawn_point_if_outside_bsp <ai>)

(<void> ai_teleport <ai> <point_reference>)

(<void> ai_bring_forward <object> <real>)

(<void> ai_renew <ai>)

(<void> ai_force_active <ai> <boolean>)

(<void> ai_force_active_by_unit <unit> <boolean>)

(<void> ai_exit_limbo <ai>)

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

(<boolean> ai_set_task <ai> <string_id> <string_id>)

(<boolean> ai_set_objective <ai> <string_id>)

(<short> ai_task_status <ai>)

(<boolean> ai_set_task_condition <ai> <boolean>)

(<boolean> ai_leadership <ai>)

(<boolean> ai_leadership_all <ai>)

(<short> ai_task_count <ai>)

(<void> ai_reset_objective <ai>)

(<void> ai_squad_patrol_objective_disallow <ai> <boolean>)

(<void> ai_place_cue <string_id>)

(<void> ai_remove_cue <string_id>)

(<boolean> generate_pathfinding)

(<void> ai_activity_set <ai> <string_id>)

(<void> ai_activity_abort <ai>)

(<void> ai_object_set_team <object> <team>)

(<void> ai_object_set_targeting_bias <object> <real>)

(<void> ai_object_set_targeting_ranges <object> <real> <real>)

(<void> ai_object_enable_targeting_from_vehicle <object> <boolean>)

(<void> ai_object_enable_grenade_attack <object> <boolean>)

(<vehicle> ai_vehicle_get <ai>)

(<vehicle> ai_vehicle_get_from_starting_location <ai>)

(<vehicle> ai_vehicle_get_from_spawn_point <ai>)

(<short> ai_vehicle_get_squad_count <ai>)

(<vehicle> ai_vehicle_get_from_squad <ai> <short>)

(<boolean> ai_vehicle_reserve_seat <vehicle> <unit_seat_mapping> <boolean>)

(<boolean> ai_vehicle_reserve <vehicle> <boolean>)

(<ai> ai_player_get_vehicle_squad <unit>)

(<short> ai_vehicle_count <ai>)

(<boolean> ai_carrying_player <ai>)

(<boolean> player_in_vehicle <vehicle>)

(<boolean> ai_player_needs_vehicle <player>)

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

(<ai> ai_squad_group_get_squad <ai> <short>)

(<short> ai_squad_group_get_squad_count <ai>)

(<void> ai_squad_patrol_enable <string_id> <boolean>)

(<short> ai_combat_status <ai>)

(<void> ai_set_combat_status <ai> <short>)

(<boolean> flock_start <string_id>)

(<boolean> flock_stop <string_id>)

(<boolean> flock_create <string_id>)

(<boolean> flock_delete <string_id>)

(<boolean> flock_destroy <string_id>)

(<boolean> flock_definition_set <string_id> <any_tag>)

(<void> flock_unperch <string_id>)

(<void> flock_set_targeting_group <string_id> <short>)

(<void> flock_destination_set_enabled <string_id> <string_id> <boolean>)

(<void> flock_destination_set_position <string_id> <string_id> <point_reference>)

(<void> flock_destination_copy_position <string_id> <string_id> <string_id>)

(<boolean> mantini_create_runtime_squad <string>)

(<boolean> mantini_create_runtime_point_set <string>)

(<boolean> mantini_add_runtime_point_to_set <string> <string> <real> <real> <real> <real>)

(<boolean> mantini_add_runtime_point_to_set_from_camera <string> <string> <real> <real>)

(<boolean> mantini_add_starting_location_to_squad <string> <string> <long> <point_reference>)

(<boolean> mantini_add_starting_location_to_squad_from_camera <string> <string> <long> <real>)

(<long> mantini_add_cell_to_runtime_squad <string> <string>)

(<long> mantini_get_runtime_squad_cell_count <string>)

(<boolean> mantini_set_squad_team <string> <team>)

(<boolean> mantini_set_spawn_point_weapons <ai> <string> <string>)

(<boolean> mantini_set_spawn_point_vehicle <ai> <string>)

(<boolean> mantini_set_spawn_point_variants <ai> <string_id> <string_id>)

(<boolean> mantini_set_squad_cell_spawn_count <string> <long> <long>)

(<boolean> mantini_remove_pointset <string>)

(<boolean> ai_wall_lean <ai>)

(<void> ai_fire_dialogue_event <ai> <string> <object>)

(<void> ai_play_vocalization <string_id>)

(<void> ai_play_vocalization_on_team <string_id> <team>)

(<real> ai_play_line <ai> <ai_line>)

(<real> ai_play_line_at_player <ai> <ai_line>)

(<real> ai_play_line_on_object <object> <ai_line>)

(<real> ai_play_line_on_object_for_team <object> <ai_line> <mp_team>)

(<short> ai_play_line_on_point_set <string_id> <point_reference> <short>)

(<short> ai_play_line_on_point_set <string_id> <point_reference> <short> <string_id>)

(<void> campaign_metagame_time_pause <boolean>)

(<void> campaign_metagame_award_points <player> <long>)

(<void> campaign_metagame_award_skull <player> <skull>)

(<boolean> campaign_metagame_enabled)

(<boolean> campaign_survival_enabled)

(<void> thespian_performance_test <string_id>)

(<void> thespian_folder_activate <folder>)

(<void> thespian_folder_deactivate <folder>)

(<void> thespian_performance_activate <string_id>)

(<boolean> thespian_performance_setup_and_begin <string_id> <string_id> <real>)

(<long> performance_new <string_id> <boolean>)

(<void> cast_squad_in_performance <long> <ai>)

(<void> performance_begin <long>)

(<void> thespian_performance_kill_by_name <string_id>)

(<void> thespian_performance_kill_by_ai <ai>)

(<void> thespian_fake_task_transition <string_id> <string_id>)

(<void> performance_play_line_by_id <short>)

(<void> performance_play_line <string_id>)

(<ai> performance_get_actor_by_index <short>)

(<ai> performance_get_actor <string_id>)

(<short> performance_get_actor_count)

(<short> performance_get_role_count)

(<short> performance_get_line_count)

(<short> performance_get_last_played_line_index)

(<boolean> thespian_performance_is_blocked)

(<ai> actor_from_performance <long> <string_id>)

(<void> ai_player_add_fireteam_squad <unit> <ai>)

(<void> ai_player_remove_fireteam_squad <unit> <ai>)

(<void> ai_player_set_fireteam_max <unit> <short>)

(<void> ai_player_set_fireteam_no_max <unit>)

(<void> ai_player_set_fireteam_max_squad_absorb_distance <unit> <real>)

(<void> ai_set_fireteam_absorber <ai> <boolean>)

(<void> ai_set_fireteam_fallback_squad <unit> <ai>)

(<boolean> ai_is_in_fireteam <ai>)

(<void> cs_run_command_script <ai> <ai_command_script>)

(<void> cs_queue_command_script <ai> <ai_command_script>)

(<void> cs_stack_command_script <ai> <ai_command_script>)

(<boolean> vs_cast <ai> <boolean> <short> <string_id>)

(<boolean> vs_cast <ai> <boolean> <short> <string_id> <string_id>)

(<boolean> vs_cast <ai> <boolean> <short> <string_id> <string_id> <string_id>)

(<boolean> vs_cast <ai> <boolean> <short> <string_id> <string_id> <string_id> <string_id>)

(<boolean> vs_cast <ai> <boolean> <short> <string_id> <string_id> <string_id> <string_id> <string_id>)

(<boolean> vs_cast <ai> <boolean> <short> <string_id> <string_id> <string_id> <string_id> <string_id> <string_id>)

(<boolean> vs_cast <ai> <boolean> <short> <string_id> <string_id> <string_id> <string_id> <string_id> <string_id> <string_id>)

(<ai> vs_role <short>)

(<void> cs_abort_on_alert <boolean>)

(<void> cs_abort_on_damage <boolean>)

(<void> cs_abort_on_combat_status <short>)

(<void> cs_abort_on_vehicle_exit <boolean>)

(<void> cs_abort_on_alert <boolean>)

(<void> cs_abort_on_alert <ai> <boolean>)

(<void> cs_abort_on_damage <boolean>)

(<void> cs_abort_on_damage <ai> <boolean>)

(<void> cs_abort_on_combat_status <short>)

(<void> cs_abort_on_combat_status <ai> <short>)

(<void> cs_abort_on_vehicle_exit <boolean>)

(<void> cs_abort_on_vehicle_exit <ai> <boolean>)

(<boolean> cs_command_script_running <ai> <ai_command_script>)

(<boolean> cs_command_script_attached <ai> <ai_command_script>)

(<short> cs_number_queued <ai>)

(<boolean> cs_moving)

(<boolean> cs_moving <ai>)

(<void> cs_fly_to <point_reference>)

(<void> cs_fly_to <ai> <boolean> <point_reference>)

(<void> cs_fly_to <point_reference> <real>)

(<void> cs_fly_to <ai> <boolean> <point_reference> <real>)

(<void> cs_fly_to_and_face <point_reference> <point_reference>)

(<void> cs_fly_to_and_face <ai> <boolean> <point_reference> <point_reference>)

(<void> cs_fly_to_and_face <point_reference> <point_reference> <real>)

(<void> cs_fly_to_and_face <ai> <boolean> <point_reference> <point_reference> <real>)

(<void> cs_fly_by <point_reference>)

(<void> cs_fly_by <ai> <boolean> <point_reference>)

(<void> cs_fly_by <point_reference> <real>)

(<void> cs_fly_by <ai> <boolean> <point_reference> <real>)

(<void> cs_go_to <point_reference>)

(<void> cs_go_to <ai> <boolean> <point_reference>)

(<void> cs_go_to <point_reference> <real>)

(<void> cs_go_to <ai> <boolean> <point_reference> <real>)

(<void> cs_go_by <point_reference> <point_reference>)

(<void> cs_go_by <ai> <boolean> <point_reference> <point_reference>)

(<void> cs_go_by <point_reference> <point_reference> <real>)

(<void> cs_go_by <ai> <boolean> <point_reference> <point_reference> <real>)

(<void> cs_go_to_and_face <point_reference> <point_reference>)

(<void> cs_go_to_and_face <ai> <boolean> <point_reference> <point_reference>)

(<void> cs_go_to_and_posture <point_reference> <string_id>)

(<void> cs_go_to_and_posture <ai> <boolean> <point_reference> <string_id>)

(<void> cs_attach_to_spline <string_id>)

(<void> cs_attach_to_spline <ai> <boolean> <string_id>)

(<void> cs_go_to_nearest <point_reference>)

(<void> cs_go_to_nearest <ai> <boolean> <point_reference>)

(<void> cs_move_in_direction <real> <real> <real>)

(<void> cs_move_in_direction <ai> <boolean> <real> <real> <real>)

(<void> cs_move_towards <object>)

(<void> cs_move_towards <ai> <boolean> <object>)

(<void> cs_move_towards <object> <boolean>)

(<void> cs_move_towards <ai> <boolean> <object> <boolean>)

(<void> cs_move_towards_point <point_reference> <real>)

(<void> cs_move_towards_point <ai> <boolean> <point_reference> <real>)

(<void> cs_swarm_to <point_reference> <real>)

(<void> cs_swarm_to <ai> <boolean> <point_reference> <real>)

(<void> cs_swarm_from <point_reference> <real>)

(<void> cs_swarm_from <ai> <boolean> <point_reference> <real>)

(<void> cs_pause <real>)

(<void> cs_pause <ai> <boolean> <real>)

(<void> cs_grenade <point_reference> <short>)

(<void> cs_grenade <ai> <boolean> <point_reference> <short>)

(<void> cs_use_equipment <real>)

(<void> cs_use_equipment <ai> <boolean> <real>)

(<void> cs_jump <real> <real>)

(<void> cs_jump <ai> <boolean> <real> <real>)

(<void> cs_jump_to_point <real> <real>)

(<void> cs_jump_to_point <ai> <boolean> <real> <real>)

(<void> cs_vocalize <short>)

(<void> cs_vocalize <ai> <boolean> <short>)

(<void> cs_play_sound <sound>)

(<void> cs_play_sound <ai> <boolean> <sound>)

(<void> cs_play_sound <sound> <real>)

(<void> cs_play_sound <ai> <boolean> <sound> <real>)

(<void> cs_play_sound <sound> <real> <real>)

(<void> cs_play_sound <ai> <boolean> <sound> <real> <real>)

(<void> cs_action <point_reference> <short>)

(<void> cs_action <ai> <boolean> <point_reference> <short>)

(<void> cs_action_at_object <object> <short>)

(<void> cs_action_at_object <ai> <boolean> <object> <short>)

(<void> cs_action_at_player <short>)

(<void> cs_action_at_player <ai> <boolean> <short>)

(<void> cs_custom_animation <animation_graph> <string_id> <boolean>)

(<void> cs_custom_animation <ai> <boolean> <animation_graph> <string_id> <boolean>)

(<void> cs_custom_animation <animation_graph> <string_id> <boolean> <point_reference>)

(<void> cs_custom_animation <ai> <boolean> <animation_graph> <string_id> <boolean> <point_reference>)

(<void> cs_custom_animation_death <animation_graph> <string_id> <boolean>)

(<void> cs_custom_animation_death <ai> <boolean> <animation_graph> <string_id> <boolean>)

(<void> cs_custom_animation_death <animation_graph> <string_id> <boolean> <point_reference>)

(<void> cs_custom_animation_death <ai> <boolean> <animation_graph> <string_id> <boolean> <point_reference>)

(<void> cs_custom_animation_loop <animation_graph> <string_id> <boolean>)

(<void> cs_custom_animation_loop <ai> <animation_graph> <string_id> <boolean>)

(<void> cs_custom_animation_loop <animation_graph> <string_id> <boolean> <point_reference>)

(<void> cs_custom_animation_loop <ai> <animation_graph> <string_id> <boolean> <point_reference>)

(<void> cs_play_line <ai_line>)

(<void> cs_play_line <ai> <boolean> <ai_line>)

(<void> cs_die <short>)

(<void> cs_die <ai> <boolean> <short>)

(<void> cs_deploy_turret <point_reference>)

(<void> cs_deploy_turret <ai> <boolean> <point_reference>)

(<void> cs_approach <object> <real> <real> <real>)

(<void> cs_approach <ai> <boolean> <object> <real> <real> <real>)

(<void> cs_approach_player <real> <real> <real>)

(<void> cs_approach_player <ai> <boolean> <real> <real> <real>)

(<void> cs_go_to_vehicle <vehicle>)

(<void> cs_go_to_vehicle <ai> <boolean> <vehicle>)

(<void> cs_go_to_vehicle <vehicle> <unit_seat_mapping>)

(<void> cs_go_to_vehicle <ai> <boolean> <vehicle> <unit_seat_mapping>)

(<void> cs_set_style <style>)

(<void> cs_set_style <ai> <style>)

(<void> cs_force_combat_status <short>)

(<void> cs_force_combat_status <ai> <short>)

(<void> cs_enable_targeting <boolean>)

(<void> cs_enable_targeting <ai> <boolean>)

(<void> cs_enable_looking <boolean>)

(<void> cs_enable_looking <ai> <boolean>)

(<void> cs_enable_moving <boolean>)

(<void> cs_enable_moving <ai> <boolean>)

(<void> cs_enable_dialogue <boolean>)

(<void> cs_enable_dialogue <ai> <boolean>)

(<void> cs_suppress_activity_termination <boolean>)

(<void> cs_suppress_activity_termination <ai> <boolean>)

(<void> cs_suppress_dialogue_global <boolean>)

(<void> cs_suppress_dialogue_global <ai> <boolean>)

(<void> cs_look <boolean> <point_reference>)

(<void> cs_look <ai> <boolean> <point_reference>)

(<void> cs_look_player <boolean>)

(<void> cs_look_player <ai> <boolean>)

(<void> cs_look_object <boolean> <object>)

(<void> cs_look_object <ai> <boolean> <object>)

(<void> cs_aim <boolean> <point_reference>)

(<void> cs_aim <ai> <boolean> <point_reference>)

(<void> cs_aim_player <boolean>)

(<void> cs_aim_player <ai> <boolean>)

(<void> cs_aim_object <boolean> <object>)

(<void> cs_aim_object <ai> <boolean> <object>)

(<void> cs_face <boolean> <point_reference>)

(<void> cs_face <ai> <boolean> <point_reference>)

(<void> cs_face_player <boolean>)

(<void> cs_face_player <ai> <boolean>)

(<void> cs_face_object <boolean> <object>)

(<void> cs_face_object <ai> <boolean> <object>)

(<void> cs_shoot <boolean>)

(<void> cs_shoot <ai> <boolean>)

(<void> cs_shoot <boolean> <object>)

(<void> cs_shoot <ai> <boolean> <object>)

(<void> cs_shoot_point <boolean> <point_reference>)

(<void> cs_shoot_point <ai> <boolean> <point_reference>)

(<void> cs_shoot_secondary_trigger <boolean>)

(<void> cs_shoot_secondary_trigger <ai> <boolean>)

(<void> cs_lower_weapon <boolean>)

(<void> cs_lower_weapon <ai> <boolean>)

(<void> cs_vehicle_speed <real>)

(<void> cs_vehicle_speed <ai> <real>)

(<void> cs_vehicle_speed_instantaneous <real>)

(<void> cs_vehicle_speed_instantaneous <ai> <real>)

(<void> cs_vehicle_boost <boolean>)

(<void> cs_vehicle_boost <ai> <boolean>)

(<void> cs_turn_sharpness <boolean> <real>)

(<void> cs_turn_sharpness <ai> <boolean> <real>)

(<void> cs_enable_pathfinding_failsafe <boolean>)

(<void> cs_enable_pathfinding_failsafe <ai> <boolean>)

(<void> cs_set_pathfinding_radius <real>)

(<void> cs_set_pathfinding_radius <ai> <real>)

(<void> cs_ignore_obstacles <boolean>)

(<void> cs_ignore_obstacles <ai> <boolean>)

(<void> cs_approach_stop)

(<void> cs_approach_stop <ai>)

(<void> cs_push_stance <string_id>)

(<void> cs_push_stance <ai> <string_id>)

(<void> cs_crouch <boolean>)

(<void> cs_crouch <ai> <boolean>)

(<void> cs_crouch <boolean> <real>)

(<void> cs_crouch <ai> <boolean> <real>)

(<void> cs_walk <boolean>)

(<void> cs_walk <ai> <boolean>)

(<void> cs_posture_set <string_id> <boolean>)

(<void> cs_posture_set <ai> <string_id> <boolean>)

(<void> cs_posture_exit)

(<void> cs_posture_exit <ai>)

(<void> cs_stow <boolean>)

(<void> cs_stow <ai> <boolean>)

(<void> cs_teleport <point_reference> <point_reference>)

(<void> cs_teleport <ai> <point_reference> <point_reference>)

(<void> cs_stop_custom_animation)

(<void> cs_stop_custom_animation <ai>)

(<void> cs_stop_sound <sound>)

(<void> cs_stop_sound <ai> <sound>)

(<void> cs_player_melee <long>)

(<void> cs_player_melee <ai> <boolean> <long>)

(<void> cs_melee_direction <long>)

(<void> cs_melee_direction <ai> <boolean> <long>)

(<void> cs_smash_direction <long>)

(<void> cs_smash_direction <ai> <boolean> <long>)

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

(<void> camera_set_cinematic_scene <cinematic_scene_definition> <long> <cutscene_flag>)

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

(<boolean> player_night_vision_on)

(<boolean> unit_night_vision_on <unit>)

(<boolean> player_active_camouflage_on)

(<boolean> player_camera_control <boolean>)

(<void> player_action_test_reset)

(<boolean> player_action_test_primary_trigger)

(<boolean> player_action_test_grenade_trigger)

(<boolean> player_action_test_vision_trigger)

(<boolean> player_action_test_rotate_weapons)

(<boolean> player_action_test_rotate_grenades)

(<boolean> player_action_test_jump)

(<boolean> player_action_test_equipment)

(<boolean> player_action_test_context_primary)

(<boolean> player_action_test_vehicle_trick_primary)

(<boolean> player_action_test_vehicle_trick_secondary)

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

(<boolean> player_action_test_dpad_up)

(<boolean> player_action_test_dpad_down)

(<boolean> player_action_test_dpad_left)

(<boolean> player_action_test_dpad_right)

(<void> unit_action_test_reset <player>)

(<boolean> unit_action_test_primary_trigger <player>)

(<boolean> unit_action_test_grenade_trigger <player>)

(<boolean> unit_action_test_vision_trigger <player>)

(<boolean> unit_action_test_rotate_weapons <player>)

(<boolean> unit_action_test_rotate_grenades <player>)

(<boolean> unit_action_test_jump <player>)

(<boolean> unit_action_test_equipment <player>)

(<boolean> unit_action_test_context_primary <player>)

(<boolean> unit_action_test_vehicle_trick_primary <player>)

(<boolean> unit_action_test_vehicle_trick_secondary <player>)

(<boolean> unit_action_test_melee <player>)

(<boolean> unit_action_test_action <player>)

(<boolean> unit_action_test_accept <player>)

(<boolean> unit_action_test_cancel <player>)

(<boolean> unit_action_test_look_relative_up <player>)

(<boolean> unit_action_test_look_relative_down <player>)

(<boolean> unit_action_test_look_relative_left <player>)

(<boolean> unit_action_test_look_relative_right <player>)

(<boolean> unit_action_test_look_relative_all_directions <player>)

(<boolean> unit_action_test_move_relative_all_directions <player>)

(<boolean> unit_action_test_cinematic_skip <player>)

(<boolean> unit_action_test_start <player>)

(<boolean> unit_action_test_back <player>)

(<boolean> unit_action_test_dpad_up <player>)

(<boolean> unit_action_test_dpad_down <player>)

(<boolean> unit_action_test_dpad_left <player>)

(<boolean> unit_action_test_dpad_right <player>)

(<void> unit_confirm_message <player>)

(<void> unit_confirm_cancel_message <player>)

(<void> unit_enable_soft_ping_region <unit> <damage_region> <boolean>)

(<void> unit_print_soft_ping_regions <unit>)

(<boolean> player0_looking_up)

(<boolean> player0_looking_down)

(<void> player_set_pitch <player> <real> <long>)

(<boolean> player_has_female_voice <player>)

(<boolean> player_action_test_lookstick_forward)

(<boolean> player_action_test_lookstick_backward)

(<void> map_reset)

(<void> switch_zone_set <zone_set>)

(<long> current_zone_set)

(<long> current_zone_set_fully_active)

(<void> game_rate <real> <real> <real>)

(<void> fade_in <real> <real> <real> <short>)

(<void> fade_out <real> <real> <real> <short>)

(<void> fade_in_for_player <player>)

(<void> fade_out_for_player <player>)

(<short> cinematic_tag_fade_out_from_game <string_id>)

(<short> cinematic_tag_fade_in_to_cinematic <string_id>)

(<short> cinematic_tag_fade_out_from_cinematic <string_id>)

(<short> cinematic_tag_fade_in_to_game <string_id>)

(<short> cinematic_transition_fade_out_from_game <cinematic_transition_definition>)

(<short> cinematic_transition_fade_in_to_cinematic <cinematic_transition_definition>)

(<short> cinematic_transition_fade_out_from_cinematic <cinematic_transition_definition>)

(<short> cinematic_transition_fade_in_to_game <cinematic_transition_definition>)

(<void> cinematic_run_script_by_name <string_id>)

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

(<void> cinematic_set <cinematic_definition>)

(<void> cinematic_set_shot <cinematic_scene_definition> <long>)

(<void> cinematic_set_early_exit <long>)

(<long> cinematic_get_early_exit)

(<void> cinematic_set_active_camera <string_id>)

(<void> cinematic_object_create <string_id>)

(<void> cinematic_object_create_cinematic_anchor <cutscene_flag>)

(<void> cinematic_object_destroy <string_id>)

(<void> cinematic_objects_destroy_all)

(<void> cinematic_destroy)

(<void> cinematic_clips_initialize_for_shot <long>)

(<void> cinematic_clips_destroy)

(<void> cinematic_lights_initialize_for_shot <long>)

(<void> cinematic_lights_destroy)

(<void> cinematic_lights_destroy_shot)

(<void> cinematic_light_object <object> <string_id> <cinematic_lightprobe> <cutscene_camera_point>)

(<void> cinematic_light_object_off <object>)

(<void> cinematic_lighting_rebuild_all)

(<void> cinematic_lighting_update_dynamic_light_direction <string> <long> <real> <real> <real>)

(<void> cinematic_lighting_update_vmf_light <string> <real> <real> <real> <real> <real> <real> <real>)

(<void> cinematic_lighting_update_analytical_light <string> <real> <real> <real> <real> <real> <real>)

(<void> cinematic_lighting_update_ambient_light <string> <real> <real> <real> <real>)

(<void> cinematic_lighting_update_scales <string> <real> <real> <real> <real>)

(<object> cinematic_object_get <string_id>)

(<unit> cinematic_unit_get <string_id>)

(<weapon> cinematic_weapon_get <string_id>)

(<void> camera_set_briefing <boolean>)

(<cinematic_definition> cinematic_tag_reference_get_cinematic <long>)

(<cinematic_scene_definition> cinematic_tag_reference_get_scene <long>)

(<effect> cinematic_tag_reference_get_effect <long> <long> <long> <long>)

(<sound> cinematic_tag_reference_get_dialogue <long> <long> <long> <long>)

(<sound> cinematic_tag_reference_get_music <long> <long> <long> <long>)

(<looping_sound> cinematic_tag_reference_get_music_looping <long> <long> <long> <long>)

(<animation_graph> cinematic_tag_reference_get_animation <long> <long> <long>)

(<boolean> cinematic_scripting_object_coop_flags_valid <long> <long>)

(<void> cinematic_scripting_fade_out <real> <real> <real> <short>)

(<void> cinematic_scripting_create_object <long> <long>)

(<void> cinematic_scripting_create_cinematic_object <long> <long>)

(<void> cinematic_scripting_start_animation <long> <long> <string_id>)

(<void> cinematic_scripting_destroy_object <long> <long>)

(<void> cinematic_scripting_destroy_objects <long>)

(<void> cinematic_scripting_create_scene <long>)

(<void> cinematic_scripting_destroy_scene <long>)

(<void> cinematic_scripting_start_effect <long> <long> <long> <object>)

(<void> cinematic_scripting_start_music <long> <long> <long>)

(<void> cinematic_scripting_start_dialogue <long> <long> <long> <object>)

(<void> cinematic_scripting_stop_music <long> <long> <long>)

(<void> cinematic_scripting_start_screen_effect <long> <long> <long>)

(<void> cinematic_scripting_stop_screen_effect <long> <long> <long>)

(<void> cinematic_scripting_create_and_animate_object <long> <long> <long> <string_id> <boolean>)

(<void> cinematic_scripting_create_and_animate_cinematic_object <long> <long> <long> <string_id> <boolean>)

(<void> cinematic_scripting_create_and_animate_object_no_animation <long> <long> <long> <boolean>)

(<void> cinematic_scripting_create_and_animate_cinematic_object_no_animation <long> <long> <long> <boolean>)

(<void> cinematic_scripting_set_user_input_constraints <long> <long> <long>)

(<void> game_won)

(<void> game_lost <boolean>)

(<void> game_revert)

(<void> game_award_level_complete_achievements)

(<boolean> game_is_cooperative)

(<boolean> game_is_playtest)

(<void> game_can_use_flashlights <boolean>)

(<void> game_insertion_point_unlock <short>)

(<void> game_insertion_point_lock <short>)

(<void> game_insertion_point_set <short>)

(<void> achievement_grant_to_player <player> <string_id>)

(<void> core_load)

(<void> core_load_name <string>)

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

(<void> survival_mode_respawn_dead_players)

(<long> survival_mode_lives_get <team>)

(<void> survival_mode_lives_set <team> <long>)

(<long> survival_mode_get_mp_round_count)

(<long> survival_mode_get_mp_round_current)

(<short> survival_mode_set_get)

(<short> survival_mode_round_get)

(<short> survival_mode_waves_per_round)

(<short> survival_mode_rounds_per_set)

(<short> survival_mode_wave_get)

(<real> survival_mode_set_multiplier_get)

(<void> survival_mode_set_multiplier_set <real>)

(<real> survival_mode_bonus_multiplier_get)

(<void> survival_mode_bonus_multiplier_set <real>)

(<long> survival_mode_get_wave_squad)

(<boolean> survival_mode_current_wave_is_initial)

(<boolean> survival_mode_current_wave_is_boss)

(<boolean> survival_mode_current_wave_is_bonus)

(<boolean> survival_mode_current_wave_is_last_in_set)

(<void> survival_mode_begin_new_set)

(<void> survival_mode_begin_new_wave)

(<void> survival_mode_end_set)

(<void> survival_mode_end_wave)

(<void> survival_mode_award_hero_medal)

(<void> survival_mode_incident_new <string_id>)

(<void> chud_bonus_round_show_timer <boolean>)

(<void> chud_bonus_round_start_timer <boolean>)

(<void> chud_bonus_round_set_timer <real>)

(<boolean> survival_mode_meets_set_start_requirements)

(<void> survival_mode_set_pregame <boolean>)

(<boolean> survival_mode_is_pregame)

(<long> survival_mode_get_time_limit)

(<long> survival_mode_get_set_count)

(<long> survival_mode_get_bonus_lives_awarded)

(<long> survival_mode_get_bonus_target)

(<long> survival_mode_get_shared_team_life_count)

(<long> survival_mode_get_elite_life_count)

(<long> survival_mode_max_lives)

(<long> survival_mode_generator_count)

(<long> survival_mode_bonus_lives_elite_death)

(<boolean> survival_mode_scenario_extras_enable)

(<boolean> survival_mode_weapon_drops_enable)

(<boolean> survival_mode_ammo_crates_enable)

(<boolean> survival_mode_waves_disabled)

(<boolean> survival_mode_generator_defend_all)

(<boolean> survival_mode_generator_random_spawn)

(<boolean> survival_mode_current_wave_uses_dropship)

(<short> survival_mode_get_current_wave_time_limit)

(<long> survival_mode_get_respawn_time_seconds <mp_team>)

(<boolean> survival_mode_team_respawns_on_wave <team>)

(<void> survival_mode_sudden_death <boolean>)

(<void> survival_increment_human_score <player>)

(<void> survival_increment_elite_score <player>)

(<void> survival_mode_set_spartan_license_plate <long> <long> <string_id> <string_id> <string_id>)

(<void> survival_mode_set_elite_license_plate <long> <long> <string_id> <string_id> <string_id>)

(<long> survival_mode_player_count_by_team <team>)

(<object_list> survival_mode_players_by_team <team>)

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

(<void> sound_looping_start_with_effect <looping_sound> <object> <real> <string_id>)

(<void> sound_looping_start <looping_sound> <object> <real>)

(<void> sound_looping_resume <looping_sound> <object> <real>)

(<void> sound_looping_stop <looping_sound>)

(<void> sound_looping_stop_immediately <looping_sound>)

(<void> sound_looping_set_scale <looping_sound> <real>)

(<void> sound_looping_set_alternate <looping_sound> <boolean>)

(<void> sound_looping_activate_layer <looping_sound> <long>)

(<void> sound_looping_deactivate_layer <looping_sound> <long>)

(<void> sound_class_set_gain <string> <real> <short>)

(<void> sound_class_set_gain_db <string> <real> <short>)

(<void> sound_class_enable_ducker <string> <boolean>)

(<void> sound_start_global_effect <string_id> <real>)

(<void> sound_start_timed_global_effect <string_id> <real> <real>)

(<void> sound_stop_global_effect <string_id>)

(<void> sound_enable_acoustic_palette <string_id>)

(<void> sound_disable_acoustic_palette <string_id>)

(<void> vehicle_force_alternate_state <vehicle> <boolean>)

(<void> vehicle_auto_turret <vehicle> <trigger_volume> <real> <real> <real>)

(<void> vehicle_hover <vehicle> <boolean>)

(<long> vehicle_count_bipeds_killed <vehicle>)

(<void> biped_ragdoll <unit>)

(<void> water_float_reset <object>)

(<void> hud_show_training_text <boolean>)

(<void> hud_set_training_text <string_id>)

(<void> hud_enable_training <boolean>)

(<void> player_training_activate_night_vision)

(<void> player_training_activate_crouch)

(<void> player_training_activate_stealth)

(<void> player_training_activate_jump)

(<void> chud_texture_cam <boolean>)

(<void> chud_show <boolean>)

(<void> chud_show_weapon_stats <boolean>)

(<void> chud_show_crosshair <boolean>)

(<void> chud_show_shield <boolean>)

(<void> chud_show_grenades <boolean>)

(<void> chud_show_messages <boolean>)

(<void> chud_show_motion_sensor <boolean>)

(<void> chud_show_cinematics <boolean>)

(<void> chud_fade_weapon_stats_for_player <player> <real> <long>)

(<void> chud_fade_crosshair_for_player <player> <real> <long>)

(<void> chud_fade_shield_for_player <player> <real> <long>)

(<void> chud_fade_grenades_for_player <player> <real> <long>)

(<void> chud_fade_messages_for_player <player> <real> <long>)

(<void> chud_fade_motion_sensor_for_player <player> <real> <long>)

(<void> chud_fade_chapter_title_for_player <player> <real> <long>)

(<void> chud_fade_cinematics_for_player <player> <real> <long>)

(<void> chud_cinematic_fade <real> <real>)

(<void> chud_track_object <object> <boolean>)

(<void> chud_track_object_with_priority <object> <long>)

(<void> chud_track_object_with_priority <object> <long> <string_id>)

(<void> chud_track_flag <cutscene_flag> <boolean>)

(<void> chud_track_flag_with_priority <cutscene_flag> <long>)

(<void> chud_track_flag_with_priority <cutscene_flag> <long> <string_id>)

(<void> chud_track_object_for_player <player> <object> <boolean>)

(<void> chud_track_object_for_player_with_priority <player> <object> <long>)

(<void> chud_track_object_for_player_with_priority <player> <object> <long> <string_id>)

(<void> chud_track_flag_for_player <player> <cutscene_flag> <boolean>)

(<void> chud_track_flag_for_player_with_priority <player> <cutscene_flag> <long>)

(<void> chud_track_flag_for_player_with_priority <player> <cutscene_flag> <long> <string_id>)

(<void> chud_track_object_set_vertical_offset <object> <real>)

(<void> chud_track_cutscene_flag_set_vertical_offset <cutscene_flag> <real>)

(<void> chud_breadcrumbs_track_flag <cutscene_flag> <boolean>)

(<void> chud_breadcrumbs_track_flag_with_priority <cutscene_flag> <long>)

(<void> chud_breadcrumbs_track_object <object> <boolean>)

(<void> chud_breadcrumbs_track_position <real> <real> <real> <string> <boolean>)

(<void> chud_breadcrumbs_track_position_with_priority <real> <real> <real> <string> <long> <boolean>)

(<void> chud_breadcrumbs_track_object_with_priority <object> <long>)

(<void> chud_breadcrumbs_track_object_with_priority <object> <long> <string_id>)

(<void> chud_breadcrumbs_track_object_set_vertical_offset <object> <real>)

(<boolean> chud_breadcrumbs_using_revised_nav_points)

(<void> chud_post_message_HACK <string>)

(<void> chud_set_static_hs_variable <player> <long> <real>)

(<void> chud_set_countdown_hs_variable <player> <long> <real>)

(<void> chud_set_countup_hs_variable <player> <long> <real>)

(<void> chud_clear_hs_variable <player> <long>)

(<void> chud_get_hs_variable <real> <player>)

(<void> chud_show_arbiter_ai_navpoint <boolean>)

(<void> chud_show_screen_objective <string_id>)

(<void> chud_show_screen_chapter_title <string_id>)

(<void> chud_show_screen_training <player> <string_id>)

(<void> player_effect_set_max_translation <real> <real> <real>)

(<void> player_effect_set_max_rotation <real> <real> <real>)

(<void> player_effect_set_max_rumble <real> <real>)

(<void> player_effect_start <real> <real>)

(<void> player_effect_stop <real>)

(<void> player_effect_set_max_translation_for_player <player> <real> <real> <real>)

(<void> player_effect_set_max_rotation_for_player <player> <real> <real> <real>)

(<void> player_effect_set_max_rumble_for_player <player> <real> <real>)

(<void> player_effect_start_for_player <player> <real> <real>)

(<void> player_effect_stop_for_player <player> <real>)

(<void> render_atmosphere_fog <boolean>)

(<void> motion_blur <boolean>)

(<void> antialias_blur <boolean>)

(<void> render_weather <boolean>)

(<void> render_patchy_fog <boolean>)

(<void> render_ssao <boolean>)

(<void> render_planar_fog <boolean>)

(<boolean> motion_blur_enabled)

(<void> render_disable_vsync <boolean>)

(<void> player_has_mad_secret_skills <long>)

(<void> player_invert_look <player>)

(<long> user_interface_controller_get_last_level_played <short>)

(<void> xoverlapped_debug_render <boolean>)

(<void> cc_enable <boolean>)

(<void> objectives_clear)

(<void> objectives_show_up_to <long>)

(<void> objectives_finish_up_to <long>)

(<void> objectives_show <long>)

(<void> objectives_finish <long>)

(<void> objectives_unavailable <long>)

(<void> objectives_secondary_show <long>)

(<void> objectives_secondary_finish <long>)

(<void> objectives_secondary_unavailable <long>)

(<void> objectives_set_string <long> <string_id>)

(<void> objectives_secondary_set_string <long> <string_id>)

(<void> objectives_show_string <string_id>)

(<void> objectives_finish_string <string_id>)

(<void> objectives_unavailable_string <string_id>)

(<void> objectives_secondary_show_string <string_id>)

(<void> objectives_secondary_finish_string <string_id>)

(<void> objectives_secondary_unavailable_string <string_id>)

(<void> input_suppress_rumble <boolean>)

(<void> play_bink_movie <string>)

(<void> play_bink_movie_from_tag <bink_definition>)

(<void> play_credits_skip_to_menu)

(<long> bink_time)

(<void> set_global_doppler_factor <real>)

(<void> set_global_mixbin_headroom <long> <long>)

(<void> data_mine_set_mission_segment <string>)

(<void> data_mine_insert <string>)

(<void> data_mine_enable <boolean>)

(<void> data_mine_debug_menu_setting <string>)

(<object_list> object_list_children <object> <object_definition>)

(<void> interpolator_start <string_id>)

(<void> interpolator_stop <string_id>)

(<void> interpolator_stop_all)

(<void> interpolator_dump <string_id>)

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

(<void> predict_animation <animation_graph> <string_id>)

(<long> mp_player_count_by_team <mp_team>)

(<object_list> mp_players_by_team <mp_team>)

(<long> mp_active_player_count_by_team <mp_team>)

(<void> deterministic_end_game)

(<void> mp_game_won <mp_team>)

(<void> mp_respawn_override_timers <mp_team>)

(<void> mp_ai_allegiance <team> <mp_team>)

(<void> mp_allegiance <mp_team> <mp_team>)

(<boolean> mp_round_started)

(<void> mp_round_end_with_winning_player <player>)

(<void> mp_round_end_with_winning_team <mp_team>)

(<void> mp_round_end)

(<void> mp_scripts_reset)

(<boolean> mover_set_program <object> <short>)

(<void> predict_bink_movie <string>)

(<void> predict_bink_movie_from_tag <bink_definition>)

(<void> camera_set_mode <unit> <long>)

(<void> camera_set_flying_cam_at_point <unit> <cutscene_camera_point>)

(<void> camera_set_target <unit> <object>)

(<void> camera_set_orbiting_cam_at_target_relative_point <unit> <string_id> <real> <real> <real>)

(<long> game_coop_player_count)

(<void> add_recycling_volume <trigger_volume> <long> <long>)

(<void> add_recycling_volume_by_type <trigger_volume> <long> <long> <long>)

(<long> game_tick_get)

(<void> saved_film_set_playback_game_speed <real>)

(<void> designer_zone_activate <designer_zone>)

(<void> designer_zone_deactivate <designer_zone>)

(<void> object_set_always_active <object> <boolean>)

(<boolean> tag_is_active <any_tag_not_resolving>)

(<void> object_set_persistent <object> <boolean>)

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

(<void> player_control_lock_gaze <player> <point_reference> <real>)

(<void> player_control_unlock_gaze <player>)

(<void> player_control_scale_all_input <real> <real>)

(<bink_definition> cinematic_tag_reference_get_bink <long>)

(<void> object_set_custom_animation_speed <object> <real>)

(<void> scenery_animation_start_at_frame_loop <scenery> <animation_graph> <string_id> <short>)

(<void> saved_film_set_repro_mode <boolean>)

(<void> font_set_emergency)

(<void> biped_force_ground_fitting_on <unit> <boolean>)

(<void> cinematic_set_chud_objective <cutscene_title>)

(<void> chud_show_cinematic_title <player> <cutscene_title>)

(<weapon> unit_get_primary_weapon <unit>)

(<animation_graph> budget_resource_get_animation_graph <animation_budget_reference>)

(<looping_sound> budget_resource_get_looping_sound <looping_sound_budget_reference>)

(<void> game_safe_to_respawn <boolean>)

(<void> game_safe_to_respawn <boolean> <player>)

(<void> ai_migrate_infanty <ai> <ai>)

(<void> render_cinematic_motion_blur <boolean>)

(<void> ai_dont_do_avoidance <ai> <boolean>)

(<void> cinematic_scripting_clean_up <long>)

(<void> ai_erase_inactive <ai> <short>)

(<void> ai_survival_cleanup <ai> <boolean> <boolean>)

(<void> ai_enable_stuck_flying_kill <ai> <boolean>)

(<void> ai_set_stuck_velocity_threshold <ai> <real>)

(<void> stop_bink_movie)

(<void> play_credits_unskippable)

(<sound> budget_resource_get_sound <sound_budget_reference>)

(<void> cinematic_set_debug_mode <boolean>)

(<object> cinematic_scripting_get_object <long> <long>)

(<void> unit_set_stance <unit> <string_id>)

(<void> skull_enable <skull> <boolean>)

(<void> allow_object_to_be_lased <object> <boolean>)

(<boolean> is_object_being_lased <object>)

(<long> simulation_profiler_detail_level <long>)

(<boolean> simulation_profiler_enable_downstream_processing <boolean>)

(<long> campaign_metagame_get_player_score <player>)

(<long> rewards_get_cookie_total <controller>)

(<void> submit_incident <string_id>)

(<void> submit_incident_with_cause_player <string_id> <player>)

(<void> submit_incident_with_cause_team <string_id> <mp_team>)

(<void> submit_incident_with_cause_player_and_effect_player <string_id> <player> <player>)

(<void> submit_incident_with_cause_player_and_effect_team <string_id> <player> <mp_team>)

(<void> submit_incident_with_cause_team_and_effect_player <string_id> <mp_team> <player>)

(<void> submit_incident_with_cause_team_and_effect_team <string_id> <mp_team> <mp_team>)

(<void> submit_incident_with_cause_campaign_team <string_id> <team>)

(<void> player_set_respawn_vehicle <player> <object_definition>)

(<boolean> teleport_players_into_vehicle <unit>)

(<void> player_set_spartan_loadout <player>)

(<void> player_set_elite_loadout <player>)

(<void> airstrike_enable <boolean>)

(<boolean> airstrike_weapons_exist)

(<void> airstrike_set_launches <long>)

(<void> game_grief_record_custom_penalty <player> <real>)

(<void> player_set_profile <starting_profile>)

(<void> player_set_profile <starting_profile> <player>)

(<boolean> unit_action_test_secondary_trigger <player>)

(<boolean> player_action_test_secondary_trigger)

(<boolean> player_machinima_camera_enable_toggle)

; AVAILABLE EXTERNAL GLOBALS:

(<boolean> enable_melee_action)

(<boolean> debug_controller_latency)

(<real> camera_fov)

(<boolean> CameraObserverAllowNonrecorderFirstPerson)

(<boolean> survival_performance_mode)

(<object> chud_debug_highlight_object)

(<real> chud_debug_highlight_object_color_red)

(<real> chud_debug_highlight_object_color_green)

(<real> chud_debug_highlight_object_color_blue)

(<real> chud_debug_highlight_object_color_alpha)

(<boolean> effects_corpse_nonviolent)

(<real> sound_global_room_gain)

(<real> sound_reverb_wet_dry_mix)

(<boolean> debug_duckers)

(<boolean> debug_sound_listeners)

(<boolean> sound_enable_expensive_obstruction)

(<boolean> sound_enable_new_obstruction_collision)

(<boolean> debug_sound_enable_multithreaded)

(<boolean> disable_expensive_physics_rebuilding)

(<real> minimum_havok_object_acceleration)

(<real> minimum_havok_biped_object_acceleration)

(<boolean> render_lightmap_shadows)

(<boolean> render_rain)

(<boolean> render_tessellated_mesh)

(<boolean> render_water_tessellated)

(<boolean> render_water_interaction)

(<boolean> render_water_refraction)

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

(<long> rasterizer_triliner_threshold)

(<long> rasterizer_present_immediate_threshold)

(<long> tiling)

(<long> render_screen_res)

(<long> render_tiling_resolve_fragment_index)

(<real> render_tiling_viewport_offset_x)

(<real> render_tiling_viewport_offset_y)

(<long> render_alpha_test_reference)

(<boolean> render_true_gamma)

(<real> render_postprocess_hue)

(<real> render_postprocess_saturation)

(<real> render_postprocess_red_filter)

(<real> render_postprocess_green_filter)

(<real> render_postprocess_blue_filter)

(<real> render_bounce_light_intensity)

(<boolean> render_bounce_light_only)

(<boolean> render_force_single_pass_rendering)

(<long> render_debug_depth_render)

(<real> render_debug_depth_render_scale_r)

(<real> render_debug_depth_render_scale_g)

(<real> render_debug_depth_render_scale_b)

(<real> water_physics_velocity_minimum)

(<real> water_physics_velocity_maximum)

(<boolean> breakable_surfaces)

(<short> ai_max_path_traverse_iterations)

(<ai> ai_current_squad)

(<ai> ai_current_actor)

(<long> ai_current_performance)

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

(<short> ai_movement_patrol)

(<short> ai_movement_asleep)

(<short> ai_movement_combat)

(<short> ai_movement_flee)

(<short> ai_movement_flaming)

(<short> ai_movement_stunned)

(<short> ai_movement_berserk)

(<boolean> e3)

(<boolean> lruv_lirp_enabled)

(<boolean> use_temp_directory_for_files)

(<boolean> enable_amortized_prediction)

(<long> amortized_prediction_object_batch_size)

(<boolean> enable_tag_resource_prediction)

(<boolean> enable_entire_pvs_prediction)

(<boolean> enable_cluster_objects_prediction)

(<boolean> ignore_predefined_performance_throttles)

(<boolean> enable_first_person_prediction)

(<boolean> enable_structure_prediction)

(<boolean> minidump_force_regular_minidump_with_ui)

(<boolean> render_alpha_to_coverage)

(<boolean> enable_sound_transmission)

(<real> motion_blur_expected_dt)

(<real> motion_blur_max)

(<real> motion_blur_scale)

(<real> motion_blur_center_falloff_x)

(<real> motion_blur_center_falloff_y)

(<real> g_fireteam_target_selection_preference_bonus)

(<real> ai_fireteam_hold_distance)

(<boolean> ai_fireteam_protect_leader)

(<boolean> force_buffer_2_frames)

(<boolean> disable_render_state_cache_optimization)

(<boolean> enable_better_cpu_gpu_sync)

(<long> motion_blur_max_viewport_count)

(<boolean> cinematic_prediction_enable)

(<boolean> skies_delete_on_zone_set_switch_enable)

(<boolean> reduce_widescreen_fov_during_cinematics)

(<boolean> respawn_players_into_friendly_vehicle)

(<boolean> display_client_synchronous_input_lag)

(<boolean> clients_always_respect_action_priority_for_weapon_switch)

(<boolean> render_pre_placed_lights)

(<boolean> mirror_context_tracking_enabled)

(<boolean> guardian_kills_awarded_to_most_damage)

(<real> shadow_apply_depth_bias)

(<real> shadow_apply_slope_depth_bias)

(<boolean> player_respawn_check_airborne)

(<real> shadow_generate_dynamic_lights_depth_bias)

(<real> shadow_generate_dynamic_lights_slope_depth_bias)

(<real> shadow_generate_depth_bias)

(<real> shadow_generate_slope_depth_bias)

(<boolean> imposter_cache_build)

(<boolean> imposter_capturing)

(<boolean> render_low_res_transparency_debug)

(<boolean> use_fmod)

(<real> rain_drop_alpha_scale)

(<boolean> saved_film_maximum_size_ignored)

(<boolean> NetBandwidthReportFromFilm)

(<boolean> NetSavedFilmBypassSimulation)

```
