(set-layouts 'tile "[]=" 'dwl:tile)
(set-layouts 'monocle "|M|" 'dwl:monocle)

;; (set-monitor-rules '())
;; (set-monitor-rules '((masters . 1)
;;                      (master-factor . 0.55)
;;                      (scale . 1)
;;                      (rr . TRANSFORM-NORMAL)
;;                      (layout . tile)))

(for-each
 (lambda (v)
   (bind 'keys
                (string-append "C-M-<F" (number->string v) ">")
                (lambda () (dwl:chvt v))))
 (iota 12 1))

(for-each
 (lambda (t)
   (bind 'keys
                (string-append "s-" (number->string t))
                (lambda () (dwl:view t)))
   (bind 'keys
                (string-append "s-S-" (number->string t))
                (lambda () (dwl:tag t))))
 (iota 9 1))

(bind 'keys "s-d" (lambda () (dwl:spawn "bemenu-run")))
(bind 'keys "s-<return>" (lambda () (dwl:spawn "foot")))

(bind 'keys "s-j" (lambda () (dwl:focus-stack 1)))
(bind 'keys "s-k" (lambda () (dwl:focus-stack -1)))

(bind 'keys "s-l" (lambda () (dwl:set-master-factor 0.05)))
(bind 'keys "s-h" (lambda () (dwl:set-master-factor -0.05)))

(bind 'keys "s-q" 'dwl:kill-client)

(bind 'keys "s-<space>" 'dwl:zoom)
(bind 'keys "s-<tab>" 'dwl:view)
(bind 'keys "s-t" (lambda () (dwl:cycle-layout 1)))
(bind 'keys "s-f" 'dwl:toggle-fullscreen)
(bind 'keys "S-s-<space>" 'dwl:toggle-floating)
(bind 'keys "S-s-<escape>" 'dwl:quit)
(bind 'keys "<XF86PowerOff>" 'dwl:quit)
(bind 'buttons "s-<mouse-left>" 'dwl:move)
(bind 'buttons "s-<mouse-middle>" 'dwl:toggle-floating)
(bind 'buttons "s-<mouse-right>" 'dwl:resize)
