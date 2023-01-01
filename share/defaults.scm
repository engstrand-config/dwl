;; Default bindings.
;; This file will only be loaded if 'inhibit-defaults? is #t.

(dwl:bind-ttys "C-M")
(dwl:bind-tags "s" "s-S")

(bind 'keys
      "s-d" (lambda () (dwl:spawn "bemenu-run"))
      "s-<return>" (lambda () (dwl:spawn "foot"))
      "s-j" (lambda () (dwl:focus-stack 1))
      "s-k" (lambda () (dwl:focus-stack -1))
      "s-l" (lambda () (dwl:set-master-factor 0.05))
      "s-h" (lambda () (dwl:set-master-factor -0.05))
      "s-q" 'dwl:kill-client
      "s-<space>" 'dwl:zoom
      "s-<tab>" 'dwl:view
      "s-t" (lambda () (dwl:cycle-layout 1))
      "s-f" 'dwl:toggle-fullscreen
      "S-s-<space>" 'dwl:toggle-floating
      "S-s-<escape>" 'dwl:quit
      "<XF86PowerOff>" 'dwl:quit)

(bind 'buttons
      "s-<mouse-left>" 'dwl:move
      "s-<mouse-middle>" 'dwl:toggle-floating
      "s-<mouse-right>" 'dwl:resize)
