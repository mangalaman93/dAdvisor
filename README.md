## dAdvisor
Dynamic Resource Control and Monitoring Framework

## Dependency
* tc for traffic shaping
* `cpulimit` for limit cpu resources for a linux process

## CmakeLists.txt file
* ADD_SUBDIRECTORY(dadvisor)

## Limitations
* Container network test
* LProcess cpu test (cpulimit is not so robust)

## TODO
* add code for setting pinned CPU
