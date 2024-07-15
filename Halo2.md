**Halo 2 Script Commands:**  
Please note that an entry being present here doesn't necessarily mean that it'll work - it may do nothing, or maybe even crash the game. 
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

(<void> sleep_forever [<script>])

(<void> sleep_until <boolean> [<short>])

(<void> wake <script name>)

(<void> inspect <expression>)

(<unit> unit <object>)

(<boolean> not <boolean>)

(<real> pin <real> <real> <real>)

(<void> print <string>)

(<object_list> players)

(<void> kill_volume_enable <trigger_volume>)

(<void> kill_volume_disable <trigger_volume>)

(<void> volume_teleport_players_not_inside <trigger_volume> <cutscene_flag>)

(<boolean> volume_test_object <trigger_volume> <object>)

(<boolean> volume_test_objects <trigger_volume> <object_list>)

(<boolean> volume_test_objects_all <trigger_volume> <object_list>)

(<object_list> volume_return_objects <trigger_volume>)

(<object_list> volume_return_objects_by_type <trigger_volume> <long>)

(<object> list_get <object_list> <short>)

(<short> list_count <object_list>)

(<short> list_count_not_dead <object_list>)

(<void> effect_new <effect> <cutscene_flag>)

(<void> effect_new_on_object_marker <effect> <object> <string_id>)

(<void> damage_new <damage> <cutscene_flag>)

(<void> damage_object <damage> <object>)

(<void> damage_objects <damage> <object_list>)

(<void> damage_players <damage>)

(<void> object_create <object_name>)

(<void> object_create_clone <object_name>)

(<void> object_create_anew <object_name>)

(<void> object_create_containing <string>)

(<void> object_create_clone_containing <string>)

(<void> object_create_anew_containing <string>)

(<void> object_destroy <object>)

(<void> object_destroy_containing <string>)

(<void> object_destroy_all)

(<void> object_destroy_type_mask <long>)

(<void> objects_delete_by_definition <object_definition>)

(<void> object_hide <object> <boolean>)

(<void> object_set_shadowless <object> <boolean>)

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

(<void> object_uses_cinematic_lighting <object> <boolean>)

(<void> cinematic_lighting_set_primary_light <real> <real> <real> <real> <real>)

(<void> cinematic_lighting_set_secondary_light <real> <real> <real> <real> <real>)

(<void> cinematic_lighting_set_ambient_light <real> <real> <real>)

(<void> objects_predict <object_list>)

(<void> objects_predict_high <object_list>)

(<void> objects_predict_low <object_list>)

(<void> object_type_predict_high <object_definition>)

(<void> object_type_predict_low <object_definition>)

(<void> object_type_predict <object_definition>)

(<void> object_teleport <object> <cutscene_flag>)

(<void> object_set_facing <object> <cutscene_flag>)

(<void> object_set_shield <object> <real>)

(<void> object_set_shield_stun <object> <real>)

(<void> object_set_shield_stun_infinite <object>)

(<void> object_set_permutation <object> <string_id> <string_id>)

(<void> object_set_region_state <object> <string_id> <model_state>)

(<boolean> objects_can_see_object <object_list> <object> <real>)

(<boolean> objects_can_see_flag <object_list> <cutscene_flag> <real>)

(<real> objects_distance_to_object <object_list> <object>)

(<real> objects_distance_to_flag <object_list> <cutscene_flag>)

(<real> objects_distance_to_position <object_list> <real> <real> <real>)

(<void> map_info)

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

(<void> texture_camera_set_object_marker <object> <string_id> <real>)

(<void> texture_camera_off)

(<void> render_lights_enable_cinematic_shadow <boolean> <object> <string_id> <real>)

(<void> scenery_animation_start <scenery> <animation_graph> <string_id>)

(<void> scenery_animation_start_loop <scenery> <animation_graph> <string_id>)

(<void> scenery_animation_start_relative <scenery> <animation_graph> <string_id> <object>)

(<void> scenery_animation_start_relative_loop <scenery> <animation_graph> <string_id> <object>)

(<void> scenery_animation_start_at_frame <scenery> <animation_graph> <string_id> <short>)

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

(<boolean> unit_is_playing_custom_animation <unit>)

(<void> object_set_custom_animations_hold_on_last_frame <boolean>)

(<void> object_set_custom_animations_prevent_lipsync_head_movement <boolean>)

(<void> unit_set_actively_controlled <unit> <boolean>)

(<short> unit_get_team_index <unit>)

(<void> unit_aim_without_turning <unit> <boolean>)

(<void> unit_set_enterable_by_player <unit> <boolean>)

(<boolean> unit_get_enterable_by_player <unit>)

(<void> unit_only_takes_damage_from_players_team <unit> <boolean>)

(<void> unit_enter_vehicle <unit> <vehicle> <string_id>)

(<void> unit_board_vehicle <unit> <string_id>)

(<void> unit_set_emotion <unit> <short>)

(<void> unit_set_emotion_animation <unit> <string_id>)

(<void> unit_set_emotional_state <unit> <string_id> <real> <short>)

(<void> unit_enable_eye_tracking <unit> <boolean>)

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

(<void> unit_lower_weapon <unit> <short>)

(<void> unit_raise_weapon <unit> <short>)

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

(<object> ai_get_object <ai>)

(<unit> ai_get_unit <ai>)

(<void> ai_attach <unit> <ai>)

(<void> ai_attach_units <object_list> <ai>)

(<void> ai_detach <unit>)

(<void> ai_detach_units <object_list>)

(<void> ai_place <ai>)

(<void> ai_place <ai> <short>)

(<void> ai_place_in_vehicle <ai> <ai>)

(<void> ai_cannot_die <ai> <boolean>)

(<boolean> ai_vitality_pinned <ai>)

(<void> ai_overcomes_oversteer <ai> <boolean>)

(<void> ai_kill <ai>)

(<void> ai_kill_silent <ai>)

(<void> ai_erase <ai>)

(<void> ai_erase_all)

(<void> ai_disposable <ai> <boolean>)

(<void> ai_select <ai>)

(<void> ai_deselect)

(<void> ai_set_deaf <ai> <boolean>)

(<void> ai_set_blind <ai> <boolean>)

(<void> ai_magically_see <ai> <ai>)

(<void> ai_magically_see_object <ai> <object>)

(<void> ai_set_active_camo <ai> <boolean>)

(<void> ai_suppress_combat <ai> <boolean>)

(<void> ai_migrate <ai> <ai>)

(<void> ai_allegiance <team> <team>)

(<void> ai_allegiance_remove <team> <team>)

(<void> ai_braindead <ai> <boolean>)

(<void> ai_braindead_by_unit <object_list> <boolean>)

(<void> ai_disregard <object_list> <boolean>)

(<void> ai_prefer_target <object_list> <boolean>)

(<void> ai_teleport_to_starting_location_if_outside_bsp <ai>)

(<void> ai_renew <ai>)

(<void> ai_force_active <ai> <boolean>)

(<void> ai_force_active_by_unit <unit> <boolean>)

(<void> ai_playfight <ai> <boolean>)

(<void> ai_reconnect)

(<void> ai_berserk <ai> <boolean>)

(<void> ai_set_team <ai> <team>)

(<void> ai_allow_dormant <ai> <boolean>)

(<boolean> ai_is_attacking <ai>)

(<short> ai_fighting_count <ai>)

(<short> ai_living_count <ai>)

(<real> ai_living_fraction <ai>)

(<real> ai_strength <ai>)

(<short> ai_swarm_count <ai>)

(<short> ai_nonswarm_count <ai>)

(<object_list> ai_actors <ai>)

(<boolean> ai_allegiance_broken <team> <team>)

(<void> ai_set_orders <ai> <ai_orders>)

(<short> ai_spawn_count <ai>)

(<ai> object_get_ai <object>)

(<boolean> ai_trigger_test <string> <ai>)

(<boolean> generate_pathfinding)

(<void> ai_render_paths_all)

(<vehicle> ai_vehicle_get <ai>)

(<vehicle> ai_vehicle_get_from_starting_location <ai>)

(<boolean> ai_vehicle_reserve_seat <vehicle> <string_id> <boolean>)

(<boolean> ai_vehicle_reserve <vehicle> <boolean>)

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

(<boolean> ai_wall_lean <ai>)

(<real> ai_play_line <ai> <string_id>)

(<real> ai_play_line_at_player <ai> <string_id>)

(<real> ai_play_line_on_object <object> <string_id>)

(<boolean> ai_scene <string_id> <ai_command_script> <ai>)

(<boolean> ai_scene <string_id> <ai_command_script> <ai> <ai>)

(<boolean> ai_scene <string_id> <ai_command_script> <ai> <ai> <ai>)

(<void> cs_run_command_script <ai> <ai_command_script>)

(<void> cs_queue_command_script <ai> <ai_command_script>)

(<void> cs_stack_command_script <ai> <ai_command_script>)

(<void> cs_run_joint_command_script <ai_command_script> <ai> <ai>)

(<void> cs_run_joint_command_script <ai_command_script> <ai> <ai> <ai>)

(<boolean> cs_command_script_running <ai> <ai_command_script>)

(<boolean> cs_command_script_queued <ai> <ai_command_script>)

(<short> cs_number_queued <ai>)

(<void> cs_switch <string_id>)

(<void> cs_switch_index <short>)

(<void> cs_transfer <ai>)

(<void> cs_transfer_stack <ai>)

(<void> cs_transfer_queue <ai>)

(<void> cs_fly_to <point reference>)

(<void> cs_fly_to <point reference> <real>)

(<void> cs_fly_to_and_face <point reference> <point reference>)

(<void> cs_fly_to_and_face <point reference> <point reference> <real>)

(<void> cs_fly_by <point reference>)

(<void> cs_fly_by <point reference> <real>)

(<void> cs_go_to <point reference>)

(<void> cs_go_to <point reference> <real>)

(<void> cs_go_by <point reference> <point reference>)

(<void> cs_go_by <point reference> <point reference> <real>)

(<void> cs_go_to_and_face <point reference> <point reference>)

(<void> cs_start_to <point reference>)

(<void> cs_go_to_nearest <point reference>)

(<boolean> cs_moving)

(<void> cs_look <boolean> <point reference>)

(<void> cs_look_player <boolean>)

(<void> cs_look_object <boolean> <object>)

(<void> cs_aim <boolean> <point reference>)

(<void> cs_aim_player <boolean>)

(<void> cs_aim_object <boolean> <object>)

(<void> cs_face <boolean> <point reference>)

(<void> cs_face_player <boolean>)

(<void> cs_face_object <boolean> <object>)

(<void> cs_move_in_direction <real> <real> <real>)

(<void> cs_pause <real>)

(<void> cs_shoot <boolean>)

(<void> cs_shoot <boolean> <object>)

(<void> cs_shoot_point <boolean> <point reference>)

(<void> cs_vehicle_speed <real>)

(<void> cs_grenade <point reference> <short>)

(<void> cs_jump <real> <real>)

(<void> cs_jump_to_point <real> <real>)

(<void> cs_vocalize <short>)

(<void> cs_play_sound <sound>)

(<void> cs_play_sound <sound> <real>)

(<void> cs_play_sound <sound> <real> <real>)

(<void> cs_stop_sound <sound>)

(<void> cs_custom_animation <animation_graph> <string_id> <real> <boolean>)

(<void> cs_stop_custom_animation)

(<void> cs_play_line <string_id>)

(<void> cs_die <short>)

(<void> cs_teleport <point reference> <point reference>)

(<void> cs_animate <long> <short>)

(<void> cs_movement_mode <short>)

(<void> cs_crouch <boolean>)

(<void> cs_crouch <boolean> <real>)

(<void> cs_set_pathfinding_radius <real>)

(<void> cs_go_to_vehicle <vehicle>)

(<void> cs_set_behavior <ai_behavior>)

(<void> cs_formation <short> <ai> <point reference> <point reference>)

(<void> cs_deploy_turret <point reference>)

(<void> cs_approach <object> <real> <real> <real>)

(<void> cs_start_approach <object> <real> <real> <real>)

(<void> cs_approach_player <real> <real> <real>)

(<void> cs_start_approach_player <real> <real> <real>)

(<void> cs_approach_stop)

(<void> cs_ignore_obstacles <boolean>)

(<void> cs_turn_sharpness <boolean> <real>)

(<void> cs_vehicle_speed_instantaneous <real>)

(<void> cs_vehicle_boost <boolean>)

(<void> cs_abort_on_alert <boolean>)

(<void> cs_abort_on_damage <boolean>)

(<void> cs_abort_on_combat_status <short>)

(<void> cs_enable_targeting <boolean>)

(<void> cs_enable_looking <boolean>)

(<void> cs_enable_moving <boolean>)

(<void> cs_enable_dialogue <boolean>)

(<void> cs_suppress_dialogue_global <boolean>)

(<void> cs_set_style <style>)

(<void> cs_force_combat_status <short>)

(<void> cs_enable_pathfinding_failsafe <boolean>)

(<void> camera_control <boolean>)

(<void> camera_set <cutscene_camera_point> <short>)

(<void> camera_set_relative <cutscene_camera_point> <short> <object>)

(<void> camera_set_animation <animation_graph> <string_id>)

(<void> camera_set_animation_relative <animation_graph> <string_id> <unit> <cutscene_flag>)

(<void> camera_predict_resources_at_frame <animation_graph> <string_id> <unit> <cutscene_flag> <long>)

(<void> camera_predict_resources_at_point <cutscene_camera_point>)

(<void> camera_set_first_person <unit>)

(<short> camera_time)

(<void> camera_set_field_of_view <real> <short>)

(<void> camera_set_pan <cutscene_camera_point> <short>)

(<void> camera_pan <cutscene_camera_point> <cutscene_camera_point> <short> <short> <real> <short> <real>)

(<game_difficulty> game_difficulty_get)

(<game_difficulty> game_difficulty_get_real)

(<void> pvs_set_object <object>)

(<void> pvs_set_camera <cutscene_camera_point>)

(<void> pvs_clear)

(<void> players_unzoom_all)

(<void> player_enable_input <boolean>)

(<void> player_disable_movement <boolean>)

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

(<boolean> player_action_test_start)

(<boolean> player_action_test_back)

(<boolean> player0_looking_up)

(<boolean> player0_looking_down)

(<void> player_action_test_look_up_begin)

(<void> player_action_test_look_down_begin)

(<void> player_action_test_look_pitch_end)

(<boolean> player_action_test_lookstick_forward)

(<boolean> player_action_test_lookstick_backward)

(<void> switch_bsp <short>)

(<void> switch_bsp_by_name <structure_bsp>)

(<short> structure_bsp_index)

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

(<void> game_won)

(<void> game_lost <boolean>)

(<void> game_revert)

(<boolean> game_is_cooperative)

(<boolean> game_is_playtest)

(<void> game_can_use_flashlights <boolean>)

(<void> game_save_unsafe)

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

(<long> sound_impulse_time <sound>)

(<long> sound_impulse_time <sound> <long>)

(<long> sound_impulse_language_time <sound>)

(<void> sound_impulse_stop <sound>)

(<void> sound_impulse_start_3d <sound> <real> <real>)

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

(<void> vehicle_hover <vehicle> <boolean>)

(<long> vehicle_count_bipeds_killed <vehicle>)

(<void> biped_ragdoll <unit>)

(<void> hud_cinematic_fade <real> <real>)

(<boolean> show_hud <boolean>)

(<boolean> show_hud_help_text <boolean>)

(<boolean> show_hud_messages <boolean>)

(<void> hud_show_training_text <boolean>)

(<void> hud_set_training_text <string_id>)

(<void> hud_enable_training <boolean>)

(<void> player_training_activate_flashlight)

(<void> player_training_activate_crouch)

(<void> player_training_activate_stealth)

(<void> activate_nav_point_flag <navpoint> <unit> <cutscene_flag> <real>)

(<void> activate_nav_point_object <navpoint> <unit> <object> <real>)

(<void> activate_team_nav_point_flag <navpoint> <team> <cutscene_flag> <real>)

(<void> activate_team_nav_point_object <navpoint> <team> <object> <real>)

(<void> deactivate_nav_point_flag <unit> <cutscene_flag>)

(<void> deactivate_nav_point_object <unit> <object>)

(<void> deactivate_team_nav_point_flag <team> <cutscene_flag>)

(<void> deactivate_team_nav_point_object <team> <object>)

(<void> breadcrumbs_activate_team_nav_point_position <navpoint> <team> <real> <real> <real> <string> <real>)

(<void> breadcrumbs_activate_team_nav_point_flag <navpoint> <team> <cutscene_flag> <real>)

(<void> breadcrumbs_activate_team_nav_point_object <navpoint> <team> <object> <real>)

(<void> breadcrumbs_deactivate_team_nav_point <team> <string>)

(<void> breadcrumbs_deactivate_team_nav_point_flag <team> <cutscene_flag>)

(<void> breadcrumbs_deactivate_team_nav_point_object <team> <object>)

(<void> player_effect_set_max_translation <real> <real> <real>)

(<void> player_effect_set_max_rotation <real> <real> <real>)

(<void> player_effect_set_max_vibration <real> <real>)

(<void> player_effect_start <real> <real>)

(<void> player_effect_stop <real>)

(<void> rasterizer_lens_flares_clear_for_frame)

(<void> rasterizer_decal_depth_bias <real>)

(<void> rasterizer_decal_depth_bias_slope_scale <real>)

(<void> rasterizer_hud_ten_foot <short>)

(<void> rasterizer_blur <short>)

(<void> script_screen_effect_set_value <short> <real>)

(<void> cinematic_screen_effect_start <boolean>)

(<void> cinematic_screen_effect_set_depth_of_field <real> <real> <real> <real> <real> <real> <real>)

(<void> cinematic_screen_effect_set_crossfade <real>)

(<void> cinematic_screen_effect_set_crossfade2 <real> <real>)

(<void> cinematic_screen_effect_stop)

(<void> cinematic_set_near_clip_distance <real>)

(<void> cinematic_set_far_clip_distance <real>)

(<void> cinematic_set_environment_map_attenuation <real> <real> <real>)

(<void> cinematic_set_environment_map_bitmap <bitmap>)

(<void> cinematic_reset_environment_map_bitmap)

(<void> cinematic_set_environment_map_tint <real> <real> <real> <real> <real> <real> <real> <real>)

(<void> cinematic_reset_environment_map_tint)

(<void> cinematic_layer <long> <real> <real>)

(<void> cinematic_dynamic_reflections <boolean> <boolean>)

(<void> controller_set_look_invert <boolean>)

(<boolean> controller_get_look_invert)

(<long> user_interface_controller_get_last_level_played <short>)

(<void> objectives_clear)

(<void> objectives_show_up_to <long>)

(<void> objectives_finish_up_to <long>)

(<void> input_suppress_vibration <boolean>)

(<void> play_credits)

(<boolean> bink_done)

(<void> set_global_doppler_factor <real>)

(<void> set_global_mixbin_headroom <long> <long>)

(<void> data_mine_set_mission_segment <string>)

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

(<void> weather_start <real>)

(<void> weather_stop <real>)

(<void> weather_change_intensity <real> <real>)

(<void> cinematic_clone_players_weapon <object> <string_id> <string_id>)

(<void> cinematic_move_attached_objects <object> <string_id> <string_id> <string_id>)

(<void> vehicle_enable_ghost_effects <boolean>)

(<void> ice_cream_flavor_stock <long>)

(<boolean> ice_cream_flavor_available <long>)

(<void> set_global_sound_environment <real> <real> <real> <real> <long> <real>)

(<void> game_save_cinematic_skip)

(<void> cinematic_outro_start)

(<void> cinematic_enable_ambience_details <boolean>)

(<void> set_rasterizer_gamma <real>)

(<void> predict_model_section <render model> <long>)

(<void> predict_structure_section <structure_bsp> <long> <boolean>)

(<void> predict_lightmap_bucket <structure_bsp> <long>)

(<void> predict_bitmap <bitmap> <long>)

(<long> get_prediction_offset)

(<void> rasterizer_bloom_override <boolean>)

(<void> rasterizer_bloom_override_reset <boolean>)

(<void> rasterizer_bloom_override_blur_amount <real>)

(<void> rasterizer_bloom_override_threshold <real>)

(<void> rasterizer_bloom_override_brightness <real>)

(<void> rasterizer_bloom_override_box_factor <real>)

(<void> rasterizer_bloom_override_max_factor <real>)

(<void> rasterizer_bloom_override_silver_bullet <boolean>)

(<void> rasterizer_bloom_override_only <boolean>)

(<void> rasterizer_bloom_override_high_res <boolean>)

(<void> rasterizer_bloom_override_brightness_alpha <real>)

(<void> rasterizer_bloom_override_max_factor_alpha <real>)

(<void> cache_block_for_one_frame)

(<void> sound_suppress_ambience_update_on_revert)

(<void> cinematic_lightmap_shadow_disable)

(<void> cinematic_lightmap_shadow_enable)

(<void> loading_screen_fade_to_white)

(<void> rasterizer_debug_frame_usage <boolean>)

(<void> rasterizer_debug_crap_pixel_shader <boolean>)

(<void> enable_render_light_suppressor)

(<void> disable_render_light_suppressor)

(<void> cinematic_start_movie <string>)

(<long> game_tick_get)

(<real> game_tick_length)

(<void> debug_menu_rebuild)

; AVAILABLE EXTERNAL GLOBALS:

(<real> camera_fov_scale)

(<boolean> disable_hud)

(<long> debug_simulate_gamepad)

(<boolean> effects_corpse_nonviolent)

(<real> sound_global_room_gain)

(<boolean> enable_pc_sound)

(<boolean> debug_sound_listeners)

(<boolean> use_dynamic_object_obstruction)

(<real> object_light_ambient_base)

(<real> object_light_ambient_scale)

(<real> object_light_secondary_scale)

(<real> object_light_secondary_direction_lerp)

(<boolean> object_light_interpolate)

(<real> object_light_accuracy_lower_bound)

(<real> object_light_accuracy_upper_bound)

(<real> object_light_accuracy_exponent)

(<boolean> object_light_new)

(<real> object_light_ambient_brightness)

(<real> object_light_primary_brightness)

(<real> object_light_secondary_brightness)

(<real> object_light_primary_exclusion_cone_lower_bound)

(<real> object_light_primary_exclusion_cone_upper_bound)

(<real> object_light_secondary_exclusion_cone_lower_bound)

(<real> object_light_secondary_exclusion_cone_upper_bound)

(<real> object_light_secondary_rotation)

(<real> texture_camera_scale)

(<boolean> debug_motion_sensor_draw_all_units)

(<boolean> breakable_surfaces)

(<boolean> decals)

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

(<short> ai_movement_patrol)

(<short> ai_movement_asleep)

(<short> ai_movement_combat)

(<short> ai_movement_flee)

(<short> ai_movement_flaming)

(<short> ai_movement_stunned)

(<short> ai_movement_berserk)

(<boolean> director_camera_switch_fast)

(<boolean> e3)

(<boolean> lruv_lirp_enabled)

(<boolean> debug_ice_cream_flavor_status_arbiter_envy)

(<boolean> debug_ice_cream_flavor_status_grunt_birthday_party)

(<boolean> debug_ice_cream_flavor_status_assassins)

(<boolean> debug_ice_cream_flavor_status_famine)

(<boolean> debug_ice_cream_flavor_status_i_would_have_been_your_daddy)

(<boolean> debug_ice_cream_flavor_status_blind)

(<boolean> debug_ice_cream_flavor_status_ghost)

(<boolean> debug_ice_cream_flavor_status_black_eye)

(<boolean> debug_ice_cream_flavor_status_catch)

(<boolean> debug_ice_cream_flavor_status_sputnik)

(<boolean> debug_ice_cream_flavor_status_mythic)

(<boolean> debug_ice_cream_flavor_status_anger)

(<boolean> debug_ice_cream_flavor_status_thunderstorm)

(<boolean> debug_ice_cream_flavor_status_iron)

(<boolean> debug_ice_cream_flavor_status_thats_just_wrong)

(<boolean> debug_ice_cream_flavor_status_bandanna)

(<boolean> debug_ice_cream_flavor_status_boom)

(<boolean> debug_ice_cream_flavor_status_eye_patch)

(<boolean> debug_ice_cream_flavor_status_fog)

(<boolean> debug_ice_cream_flavor_status_foreign)

(<boolean> debug_ice_cream_flavor_status_grunt_funeral)

(<boolean> debug_ice_cream_flavor_status_pinata)

(<boolean> debug_ice_cream_flavor_status_recession)

(<boolean> debug_ice_cream_flavor_status_malfunction)

(<boolean> debug_ice_cream_flavor_status_streaking)

(<boolean> debug_ice_cream_flavor_status_jacked)

(<boolean> debug_ice_cream_flavor_status_scarab)

(<boolean> debug_ice_cream_flavor_status_styx)

(<boolean> debug_ice_cream_flavor_status_feather)

(<boolean> debug_ice_cream_flavor_status_bonded_pair)

(<boolean> debug_ice_cream_flavor_status_masterblaster)

(<boolean> debug_ice_cream_flavor_status_so_angry)

(<boolean> debug_ice_cream_flavor_status_prophet_birthday_party)

(<boolean> debug_ice_cream_flavor_status_they_come_back)

(<boolean> debug_ice_cream_flavor_force_coop)

```