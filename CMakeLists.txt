cmake_minimum_required(VERSION 3.19)
project(grandAdventure-tsq LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets QuickWidgets)


qt_standard_project_setup()

qt_add_executable(grandAdventure-tsq
    WIN32 MACOSX_BUNDLE

    ${CMAKE_CURRENT_SOURCE_DIR}/Documents/requirements.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/Documents/Needs.txt

    function_test.h function_test.cpp
    Documents/development_logic.txt Documents/Needs.txt Documents/Project_skeleton.txt Documents/push_dee.txt Documents/requirements.txt Documents/StateMachine.png Documents/Structure.png Documents/uml.png qml/components/EnemySprite.qml qml/components/PlayerSprite.qml qml/GameCanvas.qml qml/HUD.qml qml/MenuUI.qml Resource/bgc/background.png Resource/bgc/obstacles.json Resource/role/hajimi/hajimi_idle_back_1.png Resource/role/hajimi/hajimi_idle_back_2.png Resource/role/hajimi/hajimi_idle_back_3.png Resource/role/hajimi/hajimi_idle_back_4.png Resource/role/hajimi/hajimi_idle_front_1.png Resource/role/hajimi/hajimi_idle_front_2.png Resource/role/hajimi/hajimi_idle_front_3.png Resource/role/hajimi/hajimi_idle_front_4.png Resource/role/hajimi/hajimi_idle_left_1.png Resource/role/hajimi/hajimi_idle_left_2.png Resource/role/hajimi/hajimi_idle_left_3.png Resource/role/hajimi/hajimi_idle_left_4.png Resource/role/hajimi/hajimi_idle_right_1.png Resource/role/hajimi/hajimi_idle_right_2.png Resource/role/hajimi/hajimi_idle_right_3.png Resource/role/hajimi/hajimi_idle_right_4.png Resource/role/hajimi/hajimi_run_back_1.png Resource/role/hajimi/hajimi_run_back_2.png Resource/role/hajimi/hajimi_run_back_3.png Resource/role/hajimi/hajimi_run_back_4.png Resource/role/hajimi/hajimi_run_front_1.png Resource/role/hajimi/hajimi_run_front_2.png Resource/role/hajimi/hajimi_run_front_3.png Resource/role/hajimi/hajimi_run_front_4.png Resource/role/hajimi/hajimi_run_left_1.png Resource/role/hajimi/hajimi_run_left_2.png Resource/role/hajimi/hajimi_run_left_3.png Resource/role/hajimi/hajimi_run_left_4.png Resource/role/hajimi/hajimi_run_right_1.png Resource/role/hajimi/hajimi_run_right_2.png Resource/role/hajimi/hajimi_run_right_3.png Resource/role/hajimi/hajimi_run_right_4.png Resource/role/manbo/manbo_idle_back_1.png Resource/role/manbo/manbo_idle_back_2.png Resource/role/manbo/manbo_idle_back_3.png Resource/role/manbo/manbo_idle_back_4.png Resource/role/manbo/manbo_idle_front_1.png Resource/role/manbo/manbo_idle_front_2.png Resource/role/manbo/manbo_idle_front_3.png Resource/role/manbo/manbo_idle_front_4.png Resource/role/manbo/manbo_idle_left_1.png Resource/role/manbo/manbo_idle_left_2.png Resource/role/manbo/manbo_idle_left_3.png Resource/role/manbo/manbo_idle_left_4.png Resource/role/manbo/manbo_idle_right_1.png Resource/role/manbo/manbo_idle_right_2.png Resource/role/manbo/manbo_idle_right_3.png Resource/role/manbo/manbo_idle_right_4.png Resource/role/manbo/manbo_run_back_1.png Resource/role/manbo/manbo_run_back_2.png Resource/role/manbo/manbo_run_back_3.png Resource/role/manbo/manbo_run_back_4.png Resource/role/manbo/manbo_run_front_1.png Resource/role/manbo/manbo_run_front_2.png Resource/role/manbo/manbo_run_front_3.png Resource/role/manbo/manbo_run_front_4.png Resource/role/manbo/manbo_run_left_1.png Resource/role/manbo/manbo_run_left_2.png Resource/role/manbo/manbo_run_left_3.png Resource/role/manbo/manbo_run_left_4.png Resource/role/manbo/manbo_run_right_1.png Resource/role/manbo/manbo_run_right_2.png Resource/role/manbo/manbo_run_right_3.png Resource/role/manbo/manbo_run_right_4.png Resource/role/paimeng/avatar_paimon.png Resource/role/paimeng/paimon_left_0.png Resource/role/paimeng/paimon_left_1.png Resource/role/paimeng/paimon_left_2.png Resource/role/paimeng/paimon_left_3.png Resource/role/paimeng/paimon_left_4.png Resource/role/paimeng/paimon_left_5.png Resource/role/paimeng/paimon_right_0.png Resource/role/paimeng/paimon_right_1.png Resource/role/paimeng/paimon_right_2.png Resource/role/paimeng/paimon_right_3.png Resource/role/paimeng/paimon_right_4.png Resource/role/paimeng/paimon_right_5.png Resource/role/pig/boar_left_0.png Resource/role/pig/boar_left_1.png Resource/role/pig/boar_left_2.png Resource/role/pig/boar_left_3.png Resource/role/pig/boar_left_4.png Resource/role/pig/boar_left_5.png Resource/role/pig/boar_right_0.png Resource/role/pig/boar_right_1.png Resource/role/pig/boar_right_2.png Resource/role/pig/boar_right_3.png Resource/role/pig/boar_right_4.png Resource/role/pig/boar_right_5.png Resource/role/snail/snail_left_0.png Resource/role/snail/snail_left_1.png Resource/role/snail/snail_left_2.png Resource/role/snail/snail_left_3.png Resource/role/snail/snail_left_4.png Resource/role/snail/snail_left_5.png Resource/role/snail/snail_left_6.png Resource/role/snail/snail_left_7.png Resource/role/snail/snail_right_0.png Resource/role/snail/snail_right_1.png Resource/role/snail/snail_right_2.png Resource/role/snail/snail_right_3.png Resource/role/snail/snail_right_4.png Resource/role/snail/snail_right_5.png Resource/role/snail/snail_right_6.png Resource/role/snail/snail_right_7.png Resource/role/sunflower/avatar_sunflower.png Resource/role/sunflower/sunflower_attack_ex_1.png Resource/role/sunflower/sunflower_attack_ex_2.png Resource/role/sunflower/sunflower_attack_ex_3.png Resource/role/sunflower/sunflower_attack_ex_4.png Resource/role/sunflower/sunflower_attack_ex_5.png Resource/role/sunflower/sunflower_attack_ex_6.png Resource/role/sunflower/sunflower_attack_ex_7.png Resource/role/sunflower/sunflower_attack_ex_8.png Resource/role/sunflower/sunflower_attack_ex_9.png Resource/role/sunflower/sunflower_die_1.png Resource/role/sunflower/sunflower_die_2.png Resource/role/sunflower/sunflower_idle_1.png Resource/role/sunflower/sunflower_idle_2.png Resource/role/sunflower/sunflower_idle_3.png Resource/role/sunflower/sunflower_idle_4.png Resource/role/sunflower/sunflower_idle_5.png Resource/role/sunflower/sunflower_idle_6.png Resource/role/sunflower/sunflower_idle_7.png Resource/role/sunflower/sunflower_idle_8.png Resource/role/sunflower/sunflower_run_1.png Resource/role/sunflower/sunflower_run_2.png Resource/role/sunflower/sunflower_run_3.png Resource/role/sunflower/sunflower_run_4.png Resource/role/sunflower/sunflower_run_5.png Resource/role/sunflower/sunflower_selector_background.png src/core/GameApplication.cpp src/core/GameApplication.h src/core/GameObject.cpp src/core/GameObject.h src/core/Scene.cpp src/core/Scene.h src/core/SceneManager.cpp src/core/SceneManager.h src/gameobjects/Bullet.cpp src/gameobjects/Bullet.h src/gameobjects/Character.cpp src/gameobjects/Character.h src/gameobjects/Enemy.cpp src/gameobjects/Enemy.h src/gameobjects/PaimonEnemy.cpp src/gameobjects/PaimonEnemy.h src/gameobjects/Player.cpp src/gameobjects/Player.h src/gameobjects/SurroundWeapon.cpp src/gameobjects/SurroundWeapon.h src/gameobjects/Weapon.cpp src/gameobjects/Weapon.h src/scenes/CharacterSelectScene.cpp src/scenes/CharacterSelectScene.h src/scenes/DeathScene.cpp src/scenes/DeathScene.h src/scenes/MenuScene.cpp src/scenes/MenuScene.h src/scenes/PauseScene.cpp src/scenes/PauseScene.h src/scenes/PlayScene.cpp src/scenes/PlayScene.h src/scenes/UpgradeScene.cpp src/scenes/UpgradeScene.h src/ui/GameRenderer.cpp src/ui/GameRenderer.h src/ui/MainWindow.cpp src/ui/MainWindow.h src/ui/QmlInputBridge.cpp src/ui/QmlInputBridge.h src/utils/CollisionDetection.cpp src/utils/CollisionDetection.h src/utils/MathUtils.cpp src/utils/MathUtils.h src/utils/ResourceLoader.cpp src/utils/ResourceLoader.h src/main.cpp
    src/gameobjects/exporb.h src/gameobjects/exporb.cpp



)


target_link_libraries(grandAdventure-tsq
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::QuickWidgets
       Qt6::QuickControls2
       Qt6::Quick
      Qt6::Multimedia
)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Quick Multimedia)
find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2)

include(GNUInstallDirs)

install(TARGETS grandAdventure-tsq
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET grandAdventure-tsq
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)


install(SCRIPT ${deploy_script})
