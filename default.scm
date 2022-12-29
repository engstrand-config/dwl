(for-each
 (lambda (v)
   (add-binding 'keys
                (string-append "C-M-<F" (number->string v) ">")
                (lambda () (dwl:chvt v))))
 (iota 12 1))

(for-each
 (lambda (t)
   (add-binding 'keys
                (string-append "s-" (number->string t))
                (lambda () (dwl:view t)))
   (add-binding 'keys
                (string-append "s-S-" (number->string t))
                (lambda () (dwl:tag t))))
 (iota 9 1))

(add-binding 'keys "s-d" (lambda () (dwl:spawn "bemenu-run")))
(add-binding 'keys "s-<return>" (lambda () (dwl:spawn "foot")))

(add-binding 'keys "s-j" (lambda () (dwl:focus-stack 1)))
(add-binding 'keys "s-k" (lambda () (dwl:focus-stack -1)))

(add-binding 'keys "s-l" (lambda () (dwl:set-master-factor 0.05)))
(add-binding 'keys "s-h" (lambda () (dwl:set-master-factor -0.05)))

(add-binding 'keys "s-q" 'dwl:kill-client)

(add-binding 'keys "s-<space>" 'dwl:zoom)
(add-binding 'keys "s-<tab>" 'dwl:view)
;; (add-binding 'keys "s-t" ((lambda () dwl:set-layout "tile")))
;; (add-binding 'keys "s-m" ((lambda () dwl:set-layout "monocle")))
(add-binding 'keys "s-f" 'dwl:toggle-fullscreen)
(add-binding 'keys "S-s-<space>" 'dwl:toggle-floating)
(add-binding 'keys "S-s-<escape>" 'dwl:quit)
(add-binding 'keys "<XF86PowerOff>" 'dwl:quit)
;; (add-binding 'buttons "s-<mouse-left>" 'dwl:move)
;; (add-binding 'buttons "s-<mouse-middle>" 'dwl:toggle-floating)
;; (add-binding 'buttons "s-<mouse-right>" 'dwl:resize)
