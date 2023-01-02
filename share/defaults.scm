;; Default bindings.
;; This file will only be loaded if 'inhibit-defaults? is #t.

(dwl:bind-ttys "C-M")
(dwl:bind-tags "s" "s-S")

(bind "s-d" '(dwl:spawn "bemenu-run")
      "s-<return>" '(dwl:spawn "foot")
      "s-j" '(dwl:focus-stack 1)
      "s-k" '(dwl:focus-stack -1)
      "s-l" '(dwl:set-master-factor 0.05)
      "s-h" '(dwl:set-master-factor -0.05)
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
      "s-f" 'dwl:toggle-fullscreen
      "S-s-<space>" 'dwl:toggle-floating
      "S-s-<escape>" 'dwl:quit
      "<XF86PowerOff>" 'dwl:quit
      "s-<mouse-left>" 'dwl:move
      "s-<mouse-middle>" 'dwl:toggle-floating
      "s-<mouse-right>" 'dwl:resize)
