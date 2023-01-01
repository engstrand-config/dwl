;; Helper method for starting a REPL server listening on UNIX socket.
(define (dwl:start-repl-server)
  (define socket "/tmp/dwl-guile.socket")
  (begin
    (use-modules (system repl server))
    (when (file-exists? socket) (delete-file socket))
    (spawn-server (make-unix-domain-server-socket #:path socket))
    (add-hook! dwl:hook-quit
               (lambda ()
                 (delete-file socket)
                 (stop-server-and-clients!)))))

(set 'tags (map number->string (iota 9 1)))

(set-layouts 'tile "[]=" 'dwl:tile
             'monocle "|M|" 'dwl:monocle)

;; There must be a default monitor rule (i.e. with name == NULL)
(set-monitor-rules '((masters . 1)
                     (master-factor . 0.55)
                     (scale . 1)
                     (rr . TRANSFORM-NORMAL)
                     (layout . tile)))

(for-each
 (lambda (v)
   (bind 'keys (string-append "C-M-<F" (number->string v) ">")
         (lambda () (dwl:chvt v))))
 (iota 12 1))

(for-each
 (lambda (t)
   (bind 'keys
         (string-append "s-" (number->string t)) (lambda () (dwl:view t))
         (string-append "s-S-" (number->string t)) (lambda () (dwl:tag t))))
 (iota 9 1))

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
