Import("env")
import os

def before_build_spiffs(source, target, env):
    print("Building React App...")
    env.Execute("cd frontend/solarsystemclock && npm run build_esp")
    print("React App built!")

    print("Removing old SPIFFS image...")
    env.Execute("rm -rf data")

    print("Copying React App to SPIFFS...")
    env.Execute("cp -r frontend/solarsystemclock/dist data")    

env.AddPreAction("$BUILD_DIR/spiffs.bin", before_build_spiffs)