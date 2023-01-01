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

(define (dwl:bind-ttys modifiers)
  (for-each
   (lambda (v)
     (bind (string-append modifiers "-<F" (number->string v) ">")
           (lambda () (dwl:chvt v))))
   (iota 12 1)))

(define (dwl:bind-tags view-modifiers move-modifiers)
  (for-each
   (lambda (t)
     (bind (string-append view-modifiers "-" (number->string t)) (lambda () (dwl:view t))
           (string-append move-modifiers "-" (number->string t)) (lambda () (dwl:tag t))))
   (iota 9 1)))

;; Set required options.
;; These can not be inhibited, but they can easily be overridden if needed.
(set 'tags (map number->string (iota 9 1)))
(set-layouts 'tile "[]=" 'dwl:tile)

;; There must be a default monitor rule (i.e. with name == NULL)
(set-monitor-rules '((masters . 1)
                     (master-factor . 0.55)
                     (scale . 1)
                     (rr . TRANSFORM-NORMAL)
                     (layout . tile)))
