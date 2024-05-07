#!/bin/bash
cd /home/$USER/.platformio/packages/contrib-piohome/
find . -maxdepth 1 -name "main*" -exec sed -i 's#"\\\\":"/"#"/":"/"#g' {} +