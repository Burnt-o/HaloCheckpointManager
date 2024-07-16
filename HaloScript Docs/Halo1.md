**Halo 1 Script Commands:**  
Please note that an entry being present here doesn't necessarily mean that it'll work - it may do nothing, or maybe even crash the game. 

**TODO:** Remove items from this list that don't do anything or crash the game. Feel free to make a pull request!
```
AVAILABLE FUNCTIONS:

(<passthrough> begin <expression(s)>)
; returns the last expression in a sequence after evaluating the sequence in order.

(<passthrough> begin_random <expression(s)>)
; evaluates the sequence of expressions in random order and returns the last value evaluated.

(<passthrough> if <boolean> <then> [<else>])
; returns one of two values based on the value of a condition.

(<passthrough> cond (<boolean1> <result1>) [(<boolean2> <result2>) [...]])
; returns the value associated with the first true condition.

(<passthrough> set <variable name> <expression>)
; set the value of a global variable.

(<boolean> and <boolean(s)>)
; returns true if all specified expressions are true.

(<boolean> or <boolean(s)>)
; returns true if any specified expressions are true.

(<real> + <number(s)>)
; returns the sum of all specified expressions.

(<real> - <number> <number>)
; returns the difference of two expressions.

(<real> * <number(s)>)
; returns the product of all specified expressions.

(<real> / <number> <number>)
; returns the quotient of two expressions.

(<real> min <number(s)>)
; returns the minimum of all specified expressions.

(<real> max <number(s)>)
; returns the maximum of all specified expressions.

(<boolean> = <expression> <expression>)
; returns true if two expressions are equal

(<boolean> != <expression> <expression>)
; returns true if two expressions are not equal

(<boolean> > <number> <number>)
; returns true if the first number is larger than the second.

(<boolean> < <number> <number>)
; returns true if the first number is smaller than the second.

(<boolean> >= <number> <number>)
; returns true if the first number is larger than or equal to the second.

(<boolean> <= <number> <number>)
; returns true if the first number is smaller than or equal to the second.

(<void> sleep <short> [<script>])
; pauses execution of this script (or, optionally, another script) for the specified number of ticks.

(<void> sleep_forever [<script>])
; pauses execution of this script (or, optionally, another script) forever.
NETWORK SAFE: Yes

(<void> sleep_until <boolean> [<short>])
; pauses execution of this script until the specified condition is true, checking once per second unless a different number of ticks is specified.

(<void> wake <script name>)
; wakes a sleeping script in the next update.

(<void> inspect <expression>)
; prints the value of an expression to the screen for debugging purposes.

(<unit> unit <object>)
; converts an object to a unit.

(<void> ai_debug_communication_suppress <string(s)>)
; suppresses (or stops suppressing) a set of AI communication types.

(<void> ai_debug_communication_ignore <string(s)>)
; ignores (or stops ignoring) a set of AI communication types when printing out communications.

(<void> ai_debug_communication_focus <string(s)>)
; focuses (or stops focusing) a set of unit vocalization types.

(<boolean> not <boolean>)

(<real> pin <real> <real> <real>)

(<void> debug_scripting <boolean>)

(<void> breakpoint <string>)

(<void> kill_active_scripts)

(<long> get_executing_running_thread)

(<void> kill_thread <long>)

(<boolean> script_started <string>)

(<boolean> script_finished <string>)

(<long> abs_integer <long>)

(<real> abs_real <real>)

(<long> bitwise_and <long> <long>)

(<long> bitwise_or <long> <long>)

(<long> bitwise_xor <long> <long>)

(<long> bitwise_left_shift <long> <short>)

(<long> bitwise_right_shift <long> <short>)

(<long> bit_test <long> <short>)

(<long> bit_toggle <long> <short> <boolean>)

(<long> bitwise_flags_toggle <long> <long> <boolean>)

(<void> print <string>)

(<void> print_if <boolean> <string>)

(<void> log_print <string>)

(<object_list> local_players)

(<object_list> players)

(<object_list> players_on_multiplayer_team <short>)

(<void> volume_teleport_players_not_inside <trigger_volume> <cutscene_flag>)

(<boolean> volume_test_object <trigger_volume> <object>)

(<boolean> volume_test_objects <trigger_volume> <object_list>)

(<boolean> volume_test_objects_all <trigger_volume> <object_list>)

(<void> object_teleport <object> <cutscene_flag>)

(<void> object_set_facing <object> <cutscene_flag>)

(<void> object_set_shield <object> <real>)

(<void> object_set_permutation <object> <string> <string>)

(<void> object_create <object_name>)

(<void> object_destroy <object>)

(<void> object_create_anew <object_name>)

(<void> object_create_containing <string>)

(<void> object_create_anew_containing <string>)

(<void> object_destroy_containing <string>)

(<void> object_destroy_all)

(<void> objects_delete_by_definition <object_definition>)

(<object> list_get <object_list> <short>)

(<short> list_count <object_list>)

(<short> list_count_not_dead <object_list>)

(<void> effect_new <effect> <cutscene_flag>)

(<void> effect_new_on_object_marker <effect> <object> <string>)

(<void> damage_new <damage> <cutscene_flag>)

(<void> damage_object <damage> <object>)

(<boolean> objects_can_see_object <object_list> <object> <real>)

(<boolean> objects_can_see_flag <object_list> <cutscene_flag> <real>)

(<real> objects_distance_to_object <object_list> <object>)

(<real> objects_distance_to_flag <object_list> <cutscene_flag>)

(<real> objects_distance_to_position <object_list> <real> <real> <real>)

(<void> sound_set_gain <string> <real>)

(<real> sound_get_gain <string>)

(<void> script_recompile)

(<void> script_doc)

(<void> help <string>)

(<short> random_range <short> <short>)

(<real> real_random_range <real> <real>)

(<void> physics_constants_reset)

(<void> physics_set_gravity <real>)

(<real> physics_get_gravity)

(<void> numeric_countdown_timer_set <long> <boolean>)

(<short> numeric_countdown_timer_get <short>)

(<void> numeric_countdown_timer_stop)

(<void> numeric_countdown_timer_restart)

(<void> breakable_surfaces_enable <boolean>)

(<boolean> recording_play <unit> <cutscene_recording>)

(<boolean> recording_play_and_delete <unit> <cutscene_recording>)

(<boolean> recording_play_and_hover <vehicle> <cutscene_recording>)

(<void> recording_kill <unit>)

(<short> recording_time <unit>)

(<void> object_set_ranged_attack_inhibited <object> <boolean>)

(<void> object_set_melee_attack_inhibited <object> <boolean>)

(<void> objects_dump_memory)

(<void> object_set_collideable <object> <boolean>)

(<void> object_set_scale <object> <real> <short>)

(<void> objects_attach <object> <string> <object> <string>)

(<void> objects_detach <object> <object>)

(<void> garbage_collect_now)

(<void> object_cannot_take_damage <object_list>)

(<void> object_can_take_damage <object_list>)

(<void> object_beautify <object> <boolean>)

(<void> objects_predict <object_list>)

(<void> object_type_predict <object_definition>)

(<void> object_pvs_activate <object>)

(<void> object_pvs_set_object <object>)

(<void> object_pvs_set_camera <cutscene_camera_point>)

(<void> object_pvs_clear)

(<boolean> render_lights <boolean>)

(<short> scenery_get_animation_time <scenery>)

(<void> scenery_animation_start <scenery> <animation_graph> <string>)

(<void> scenery_animation_start_at_frame <scenery> <animation_graph> <string> <short>)

(<void> unit_can_blink <unit> <boolean>)

(<void> unit_open <unit>)

(<void> unit_close <unit>)

(<void> unit_kill <unit>)

(<void> unit_kill_silent <unit>)

(<short> unit_get_custom_animation_time <unit>)

(<void> unit_stop_custom_animation <unit>)

(<boolean> unit_custom_animation_at_frame <unit> <animation_graph> <string> <boolean> <short>)

(<boolean> custom_animation <unit> <animation_graph> <string> <boolean>)

(<boolean> custom_animation_list <object_list> <animation_graph> <string> <boolean>)

(<boolean> unit_is_playing_custom_animation <unit>)

(<void> unit_aim_without_turning <unit> <boolean>)

(<void> unit_set_emotion <unit> <short>)

(<void> unit_set_enterable_by_player <unit> <boolean>)

(<void> unit_enter_vehicle <unit> <vehicle> <string>)

(<boolean> vehicle_test_seat_list <vehicle> <string> <object_list>)

(<boolean> vehicle_test_seat <vehicle> <string> <unit>)

(<void> unit_set_emotion_animation <unit> <string>)

(<void> unit_exit_vehicle <unit>)

(<void> unit_set_maximum_vitality <unit> <real> <real>)

(<void> units_set_maximum_vitality <object_list> <real> <real>)

(<void> unit_set_current_vitality <unit> <real> <real>)

(<void> units_set_current_vitality <object_list> <real> <real>)

(<short> vehicle_load_magic <unit> <string> <object_list>)

(<short> vehicle_unload <unit> <string>)

(<void> magic_seat_name <string>)

(<void> unit_set_seat <unit> <string>)

(<void> magic_melee_attack)

(<object_list> vehicle_riders <unit>)

(<unit> vehicle_driver <unit>)

(<unit> vehicle_gunner <unit>)

(<real> unit_get_health <unit>)

(<real> unit_get_shield <unit>)

(<short> unit_get_total_grenade_count <unit>)

(<boolean> unit_has_weapon <unit> <object_definition>)

(<boolean> unit_has_weapon_readied <unit> <object_definition>)

(<void> unit_doesnt_drop_items <object_list>)

(<void> unit_impervious <object_list> <boolean>)

(<void> unit_suspended <unit> <boolean>)

(<boolean> unit_solo_player_integrated_night_vision_is_active)

(<void> units_set_desired_flashlight_state <object_list> <boolean>)

(<void> unit_set_desired_flashlight_state <unit> <boolean>)

(<boolean> unit_get_current_flashlight_state <unit>)

(<void> device_set_never_appears_locked <device> <boolean>)

(<real> device_get_power <device>)

(<void> device_set_power <device> <real>)

(<boolean> device_set_position <device> <real>)

(<real> device_get_position <device>)

(<void> device_set_position_immediate <device> <real>)

(<real> device_group_get <device_group>)

(<boolean> device_group_set <device_group> <real>)

(<void> device_group_set_immediate <device_group> <real>)

(<void> device_one_sided_set <device> <boolean>)

(<void> device_operates_automatically_set <device> <boolean>)

(<void> device_group_change_only_once_more_set <device_group> <boolean>)

(<void> breakable_surfaces_reset)

(<void> cheat_all_powerups)

(<void> cheat_all_weapons)

(<void> cheat_give_some_gun)

(<void> cheat_all_chars)

(<void> cheat_spawn_warthog)

(<void> cheat_all_vehicles)

(<void> cheat_teleport_to_camera)

(<void> cheat_active_camouflage)

(<void> cheat_active_camouflage_local_player <short>)

(<void> ai_free <ai>)

(<void> ai_free_units <object_list>)

(<void> ai_attach <unit> <ai>)

(<void> ai_attach_free <unit> <actor_variant>)

(<void> ai_detach <unit>)

(<void> ai_place <ai>)

(<void> ai_kill <ai>)

(<void> ai_kill_silent <ai>)

(<void> ai_erase <ai>)

(<void> ai_erase_all)

(<void> ai_select <ai>)

(<void> ai_spawn_actor <ai>)

(<void> ai_set_respawn <ai> <boolean>)

(<void> ai_set_deaf <ai> <boolean>)

(<void> ai_set_blind <ai> <boolean>)

(<void> ai_magically_see_encounter <ai> <ai>)

(<void> ai_magically_see_players <ai>)

(<void> ai_magically_see_unit <ai> <unit>)

(<void> ai_timer_start <ai>)

(<void> ai_timer_expire <ai>)

(<void> ai_attack <ai>)

(<void> ai_defend <ai>)

(<void> ai_retreat <ai>)

(<void> ai_maneuver <ai>)

(<void> ai_maneuver_enable <ai> <boolean>)

(<void> ai_migrate <ai> <ai>)

(<void> ai_migrate_and_speak <ai> <ai> <string>)

(<void> ai_migrate_by_unit <object_list> <ai>)

(<void> ai_allegiance <team> <team>)

(<void> ai_allegiance_remove <team> <team>)

(<short> ai_living_count <ai>)

(<real> ai_living_fraction <ai>)

(<real> ai_strength <ai>)

(<short> ai_swarm_count <ai>)

(<short> ai_nonswarm_count <ai>)

(<object_list> ai_actors <ai>)

(<void> ai_go_to_vehicle <ai> <unit> <string>)

(<void> ai_go_to_vehicle_override <ai> <unit> <string>)

(<short> ai_going_to_vehicle <unit>)

(<void> ai_exit_vehicle <ai>)

(<void> ai_braindead <ai> <boolean>)

(<void> ai_braindead_by_unit <object_list> <boolean>)

(<void> ai_disregard <object_list> <boolean>)

(<void> ai_prefer_target <object_list> <boolean>)

(<void> ai_teleport_to_starting_location <ai>)

(<void> ai_teleport_to_starting_location_if_unsupported <ai>)

(<void> ai_renew <ai>)

(<void> ai_try_to_fight_nothing <ai>)

(<void> ai_try_to_fight <ai> <ai>)

(<void> ai_try_to_fight_player <ai>)

(<void> ai_command_list <ai> <ai_command_list>)

(<void> ai_command_list_by_unit <unit> <ai_command_list>)

(<void> ai_command_list_advance <ai>)

(<void> ai_command_list_advance_by_unit <unit>)

(<short> ai_command_list_status <object_list>)

(<boolean> ai_is_attacking <ai>)

(<void> ai_force_active <ai> <boolean>)

(<void> ai_force_active_by_unit <unit> <boolean>)

(<void> ai_set_return_state <ai> <ai_default_state>)

(<void> ai_set_current_state <ai> <ai_default_state>)

(<void> ai_playfight <ai> <boolean>)

(<short> ai_status <ai>)

(<void> ai_vehicle_encounter <unit> <ai>)

(<void> ai_vehicle_enterable_distance <unit> <real>)

(<void> ai_vehicle_enterable_team <unit> <team>)

(<void> ai_vehicle_enterable_actor_type <unit> <actor_type>)

(<void> ai_vehicle_enterable_actors <unit> <ai>)

(<void> ai_vehicle_enterable_disable <unit>)

(<void> ai_look_at_object <unit> <object>)

(<void> ai_stop_looking <unit>)

(<void> ai_automatic_migration_target <ai> <boolean>)

(<void> ai_follow_target_disable <ai>)

(<void> ai_follow_target_players <ai>)

(<void> ai_follow_target_unit <ai> <unit>)

(<void> ai_follow_target_ai <ai> <ai>)

(<void> ai_follow_distance <ai> <real>)

(<boolean> ai_conversation <conversation>)

(<void> ai_conversation_stop <conversation>)

(<void> ai_conversation_advance <conversation>)

(<short> ai_conversation_line <conversation>)

(<short> ai_conversation_status <conversation>)

(<void> ai_link_activation <ai> <ai>)

(<void> ai_berserk <ai> <boolean>)

(<void> ai_set_team <ai> <team>)

(<void> ai_allow_charge <ai> <boolean>)

(<void> ai_allow_dormant <ai> <boolean>)

(<boolean> ai_allegiance_broken <team> <team>)

(<void> camera_control <boolean>)

(<void> camera_set <cutscene_camera_point> <short>)

(<void> camera_set_relative <cutscene_camera_point> <short> <object>)

(<void> camera_set_animation <animation_graph> <string>)

(<void> camera_set_first_person <unit>)

(<void> camera_set_dead <unit>)

(<short> camera_time)

(<void> debug_camera_load)

(<void> debug_camera_save)

(<void> debug_camera_save_name <string>)

(<void> debug_camera_load_name <string>)

(<void> debug_camera_save_simple_name <string>)

(<void> debug_camera_load_simple_name <string>)

(<void> debug_camera_load_text <string>)

(<long> game_time)

(<long> game_time_authoritative)

(<void> game_variant <string>)

(<game_difficulty> game_difficulty_get)

(<game_difficulty> game_difficulty_get_real)

(<void> map_reset)

(<void> multiplayer_map_name <string>)

(<void> game_difficulty_set <game_difficulty>)

(<void> crash <string>)

(<void> switch_bsp <short>)

(<short> structure_bsp_index)

(<void> version)

(<void> playback)

(<void> quit)

(<void> ai <boolean>)

(<void> ai_dialogue_triggers <boolean>)

(<void> ai_grenades <boolean>)

(<void> fade_in <real> <real> <real> <short>)

(<void> fade_out <real> <real> <real> <short>)

(<void> cinematic_start)

(<void> cinematic_stop)

(<void> cinematic_abort)

(<void> cinematic_skip_start_internal)

(<void> cinematic_skip_stop_internal)

(<void> cinematic_show_letterbox <boolean>)

(<void> cinematic_set_title <cutscene_title>)

(<void> cinematic_set_title_delayed <cutscene_title> <real>)

(<void> cinematic_suppress_bsp_object_creation <boolean>)

(<void> game_won)

(<void> game_lost)

(<boolean> game_safe_to_save)

(<boolean> game_all_quiet)

(<boolean> game_safe_to_speak)

(<boolean> game_is_cooperative)

(<boolean> game_is_authoritative)

(<void> game_save)

(<void> game_save_cancel)

(<void> game_save_no_timeout)

(<void> game_save_totally_unsafe)

(<boolean> game_saving)

(<void> game_revert)

(<boolean> game_reverted)

(<void> core_save)

(<boolean> core_save_name <string>)

(<void> core_load)

(<void> core_load_at_startup)

(<void> core_load_name <string>)

(<void> core_load_name_at_startup <string>)

(<boolean> mcc_mission_segment <string>)

(<void> game_skip_ticks <short>)

(<void> sound_impulse_predict <sound> <boolean>)

(<void> sound_impulse_start <sound> <object> <real>)

(<long> sound_impulse_time <sound>)

(<void> sound_impulse_stop <sound>)

(<void> sound_looping_predict <looping_sound>)

(<void> sound_looping_start <looping_sound> <object> <real>)

(<void> sound_looping_stop <looping_sound>)

(<void> sound_looping_set_scale <looping_sound> <real>)

(<void> sound_looping_set_alternate <looping_sound> <boolean>)

(<void> debug_sounds_enable <string> <boolean>)

(<void> sound_enable <boolean>)

(<void> sound_set_master_gain <real>)

(<real> sound_get_master_gain)

(<void> sound_set_music_gain <real>)

(<real> sound_get_music_gain)

(<void> sound_set_effects_gain <real>)

(<real> sound_get_effects_gain)

(<void> sound_class_set_gain <string> <real> <short>)

(<void> vehicle_hover <vehicle> <boolean>)

(<void> players_unzoom_all)

(<void> player_enable_input <boolean>)

(<boolean> player_camera_control <boolean>)

(<void> player_action_test_reset)

(<boolean> player_action_test_jump)

(<boolean> player_action_test_primary_trigger)

(<boolean> player_action_test_grenade_trigger)

(<boolean> player_action_test_zoom)

(<boolean> player_action_test_action)

(<boolean> player_action_test_accept)

(<boolean> player_action_test_back)

(<boolean> player_action_test_look_relative_up)

(<boolean> player_action_test_look_relative_down)

(<boolean> player_action_test_look_relative_left)

(<boolean> player_action_test_look_relative_right)

(<boolean> player_action_test_look_relative_all_directions)

(<boolean> player_action_test_move_relative_all_directions)

(<void> player_add_equipment <unit> <starting_profile> <boolean>)

(<boolean> show_hud <boolean>)

(<boolean> show_hud_help_text <boolean>)

(<void> enable_hud_help_flash <boolean>)

(<void> hud_help_flash_restart)

(<void> activate_nav_point_flag <navpoint> <unit> <cutscene_flag> <real>)

(<void> activate_nav_point_object <navpoint> <unit> <object> <real>)

(<void> activate_team_nav_point_flag <navpoint> <team> <cutscene_flag> <real>)

(<void> activate_team_nav_point_object <navpoint> <team> <object> <real>)

(<void> breadcrumbs_activate_team_nav_point_position <navpoint> <team> <real> <real> <real> <string> <real>)

(<void> breadcrumbs_activate_team_nav_point_flag <navpoint> <team> <cutscene_flag> <real>)

(<void> breadcrumbs_activate_team_nav_point_object <navpoint> <team> <object> <real>)

(<void> deactivate_nav_point_flag <unit> <cutscene_flag>)

(<void> deactivate_nav_point_object <unit> <object>)

(<void> deactivate_team_nav_point_flag <team> <cutscene_flag>)

(<void> deactivate_team_nav_point_object <team> <object>)

(<void> breadcrumbs_deactivate_team_nav_point <team> <string>)

(<void> breadcrumbs_deactivate_team_nav_point_flag <team> <cutscene_flag>)

(<void> breadcrumbs_deactivate_team_nav_point_object <team> <object>)

(<boolean> breadcrumbs_nav_points_active)

(<void> cls)

(<void> error_overflow_suppression <boolean>)

(<void> player_effect_set_max_translation <real> <real> <real>)

(<void> player_effect_set_max_rotation <real> <real> <real>)

(<void> player_effect_set_max_vibrate <real> <real>)

(<void> player_effect_set_max_rumble <real> <real>)

(<void> player_effect_start <real> <real>)

(<void> player_effect_stop <real>)

(<void> hud_show_health <boolean>)

(<void> hud_blink_health <boolean>)

(<void> hud_show_shield <boolean>)

(<void> hud_blink_shield <boolean>)

(<void> hud_show_motion_sensor <boolean>)

(<void> hud_blink_motion_sensor <boolean>)

(<void> hud_show_crosshair <boolean>)

(<void> hud_clear_messages)

(<void> hud_set_help_text <hud_message>)

(<void> hud_set_objective_text <hud_message>)

(<void> hud_set_timer_time <short> <short>)

(<void> hud_set_timer_warning_time <short> <short>)

(<void> hud_set_timer_position <short> <short> <hud_corner>)

(<void> show_hud_timer <boolean>)

(<void> pause_hud_timer <boolean>)

(<short> hud_get_timer_ticks)

(<void> rasterizer_model_ambient_reflection_tint <real> <real> <real> <real>)

(<void> rasterizer_lights_reset_for_new_map)

(<void> script_screen_effect_set_value <short> <real>)

(<void> cinematic_screen_effect_start <boolean>)

(<void> cinematic_screen_effect_set_convolution <short> <short> <real> <real> <real>)

(<void> cinematic_screen_effect_set_filter <real> <real> <real> <real> <boolean> <real>)

(<void> cinematic_screen_effect_set_filter_desaturation_tint <real> <real> <real>)

(<void> cinematic_screen_effect_set_video <short> <real>)

(<void> cinematic_screen_effect_stop)

(<void> cinematic_set_near_clip_distance <real>)

(<void> player0_look_invert_pitch <boolean>)

(<boolean> player0_look_pitch_is_inverted)

(<boolean> player0_joystick_set_is_normal)

(<void> ui_widget_show_path <boolean>)

(<void> display_scenario_help <short>)

(<real> get_yaw_rate <short>)

(<real> get_pitch_rate <short>)

(<void> set_yaw_rate <short> <real>)

(<void> set_pitch_rate <short> <real>)

(<void> bind <string> <string> <string>)

(<void> unbind <string> <string>)

(<void> print_binds)

(<void> sv_map <string> <string>)

(<void> profile_load <string>)

(<void> checkpoint_save)

(<void> checkpoint_load <string>)

; AVAILABLE EXTERNAL GLOBALS:

(<boolean> rasterizer_wireframe)

(<boolean> rasterizer_environment_diffuse_textures)

(<boolean> rasterizer_fog_atmosphere)

(<boolean> rasterizer_fog_plane)

(<short> rasterizer_effects_level)

(<boolean> rasterizer_fps)

(<boolean> debug_no_frustum_clip)

(<boolean> debug_frustum)

(<short> player_spawn_count)

(<boolean> debug_render_freeze)

(<boolean> breadcrumbs_navpoints_enabled_override)

(<boolean> cheat_jetpack)

(<boolean> cheat_infinite_ammo)

(<boolean> cheat_bottomless_clip)

(<boolean> cheat_bump_possession)

(<boolean> cheat_super_jump)

(<boolean> cheat_reflexive_damage_effects)

(<boolean> cheat_medusa)

(<boolean> cheat_omnipotent)

(<boolean> cheat_controller)

(<boolean> effects_corpse_nonviolent)

(<boolean> debug_sound_cache)

(<boolean> debug_sound_cache_graph)

(<real> sound_obstruction_ratio)

(<boolean> debug_sound)

(<boolean> debug_looping_sound)

(<boolean> debug_sound_channels)

(<boolean> debug_sound_channels_detail)

(<boolean> debug_sound_hardware)

(<boolean> loud_dialog_hack)

(<real> sound_gain_under_dialog)

(<real> object_light_ambient_base)

(<real> object_light_ambient_scale)

(<real> object_light_secondary_scale)

(<boolean> object_light_interpolate)

(<boolean> model_animation_compression)

(<boolean> rider_ejection)

(<boolean> stun_enable)

(<boolean> debug_motion_sensor_draw_all_units)

(<boolean> decals)

(<boolean> weather)

(<boolean> breakable_surfaces)

(<boolean> decals)

(<boolean> recover_saved_games_hack)

(<boolean> structures_use_pvs_for_vs)

(<boolean> controls_enable_crouch)

(<boolean> controls_swapped)

(<boolean> controls_enable_doubled_spin)

(<boolean> controls_swap_doubled_spin_state)

(<boolean> player_autoaim)

(<boolean> player_magnetism)

(<boolean> director_camera_switch_fast)

(<boolean> director_camera_switching)

(<boolean> debug_framerate)

(<boolean> display_framerate)

(<boolean> framerate_throttle)

(<boolean> framerate_lock)

(<boolean> error_suppress_all)

(<long> network_connect_timeout)

(<long> debug_score)

(<boolean> object_prediction)

(<short> developer_mode)

(<real> mouse_acceleration)

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

(<boolean> debug_ice_cream_flavor_status_styx)

(<boolean> debug_ice_cream_flavor_status_feather)

(<boolean> debug_ice_cream_flavor_status_bonded_pair)

(<boolean> debug_ice_cream_flavor_status_masterblaster)

(<boolean> debug_ice_cream_flavor_force_coop)

(<boolean> game_paused)
```
