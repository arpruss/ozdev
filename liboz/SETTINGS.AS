
psect lowpage
global $settings
global __ozkeyrepeatspeed,__ozkeyrepeatdelay
global __ozclick_setting
global __ozautoofftime,__ozautoblanktime,__ozautolightofftime
global __ozprogoptions

$settings:
;; the settings block is 14 bytes long
__ozkeyrepeatspeed:
        defb 5
__ozkeyrepeatdelay:
        defb 32
__ozclick_setting:
        defb 1
__ozautoofftime:
        defw 60*180 ;; 3 hours
__ozautoblanktime:
        defw 240    ;; 4 minutes
__ozautolightofftime:
        defw 120    ;; 2 minutes
__ozprogoptions:
        defb 0
;; padding (for future expansion)
defb 0,0,0
defb 0

