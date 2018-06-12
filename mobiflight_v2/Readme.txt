MobiFlight v2 -
Main source folder

This is the current main folder for v2.
It has been derived from mobiflight_mega with following changes:
- MF_registration.inc changed to .cpp as own module to avoid the need for raw source inclusion
- Constant declarations moved from mobiflight.ino to mobiflight.h (which earlier contained only fn prototypes for IDE compilation; these prototypes are still there, but guarded by an #ifdef switch)
- Part of the code in mobiflight.ino moved to mobiflight.cpp in order to make the .ino smaller.

