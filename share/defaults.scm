;; Default bindings.
;; This file will only be loaded if 'inhibit-defaults? is #f.

(dwl:set-tty-keys "C-M")
(dwl:set-tag-keys "s" "s-S")

(set-keys "s-d" '(dwl:spawn "bemenu-run")
          "s-<return>" '(dwl:spawn "foot")
          "s-j" '(dwl:focus-stack 1)
          "s-k" '(dwl:focus-stack -1)
          "s-l" '(dwl:change-master-factor 0.05)
          "s-h" '(dwl:change-master-factor -0.05)
          "s-<page-up>" '(dwl:change-masters 1)
          "s-<page-down>" '(dwl:change-masters -1)
          "s-t" '(dwl:cycle-layout 1)
          "s-<left>" '(dwl:focus-monitor 'DIRECTION-LEFT)
          "s-<right>" '(dwl:focus-monitor 'DIRECTION-RIGHT)
          "s-<up>" '(dwl:focus-monitor 'DIRECTION-UP)
          "s-<down>" '(dwl:focus-monitor 'DIRECTION-DOWN)
          "s-S-<left>" '(dwl:tag-monitor 'DIRECTION-LEFT)
          "s-S-<right>" '(dwl:tag-monitor 'DIRECTION-RIGHT)
          "s-S-<up>" '(dwl:tag-monitor 'DIRECTION-UP)
          "s-S-<down>" '(dwl:tag-monitor 'DIRECTION-DOWN)
          "s-q" 'dwl:kill-client
          "s-<space>" 'dwl:zoom
          "s-<tab>" 'dwl:view
          "s-S-0" '(dwl:view 0) ;; 0 will show all tags
          "s-f" 'dwl:toggle-fullscreen
          "S-s-<space>" 'dwl:toggle-floating
          "S-s-<escape>" 'dwl:quit
          "<XF86PowerOff>" 'dwl:quit
          "s-<mouse-left>" 'dwl:move
          "s-<mouse-middle>" 'dwl:toggle-floating
          "s-<mouse-right>" 'dwl:resize)
