(define %dwl:repl-socket-path "/tmp/dwl-guile.socket")

(define* (dwl:start-repl-server)
  "Starts a local Guile REPL server, listening on a UNIX socket at path
@path{/tmp/dwl-guile.socket}. This REPL allows you to execute expressions
in the dwl-guile context, just like @code{dwl-guile -e \"<exp\"}, but with
a more user-friendly interface.

The preferred way of connecting to the REPL server is using Geiser in Emacs.
You can connect to the server by calling @code{geiser-connect-local}, and
specifying the UNIX-socket path.

Note that this needs to be explicitly called in order for the REPL server to
be started!
"
  (begin
    (use-modules (system repl server))
    (when (file-exists? %dwl:repl-socket-path)
      (delete-file %dwl:repl-socket-path))
    (spawn-server (make-unix-domain-server-socket #:path %dwl:repl-socket-path))
    (add-hook! dwl:hook-quit
               (lambda ()
                 (delete-file %dwl:repl-socket-path)
                 (stop-server-and-clients!)))))

(define* (dwl:list-options)
  "Lists all available options that can be configured using the @code{set}
procedure."
  (hash-fold
   (lambda (key value acc)
     ;; Discard value since it just contains C-related metadata
     (cons key acc))
   '()
   %_DWL_METADATA))

(define* (dwl:list-keysyms)
  "Lists all available keysyms and their respective keycode that can be used
when binding keys and buttons using the @code{bind} procedure."
  (define (iterator key value acc)
     (cons `(,key ,value) acc))

  (hash-fold iterator
             (hash-fold iterator '() %_DWL_KEYCODES)
             %_DWL_KEYCODES_MOUSE))

(define* (dwl:show-options)
  "Same as @code{dwl:list-options}, but the list of options are printed
in a readable format."
  (for-each
   (lambda (option) (display (string-append (symbol->string option) "\n")))
   (sort-list (dwl:list-options)
              (lambda (x y) (string< (symbol->string x)
                                     (symbol->string y))))))

(define* (dwl:show-keysyms)
  "Same as @code{dwl:list-keysyms}, but the list of keysyms are printed
in a readable format."
  (for-each
   (lambda (pair)
     (display (string-append (car pair) " = " (number->string (cadr pair)) "\n")))
   (sort-list (dwl:list-keysyms)
              (lambda (x y) (< (cadr x) (cadr y))))))

(define* (dwl:bind-ttys modifiers #:optional (ttys 12))
  "Helper procedure for binding all ttys to MODIFIERS + F[1-TTYS]."
  (for-each
   (lambda (v)
     (bind (string-append modifiers "-<F" (number->string v) ">") `(dwl:chvt ,v)))
   (iota ttys 1)))

(define* (dwl:bind-tags view-modifiers move-modifiers #:optional (tags 9))
  "Helper procedure for adding bindings for viewing and moving clients to
tags 1-TAGS. The key modifiers used for viewing and moving can be set by
VIEW-MODIFIERS, and MOVE-MODIFIERS, respectively."
  (for-each
   (lambda (t)
     (bind (string-append view-modifiers "-" (number->string t)) `(dwl:view ,t)
           (string-append move-modifiers "-" (number->string t)) `(dwl:tag ,t)))
   (iota tags 1)))

;; Set required options.
;; These can not be inhibited, but they can easily be overridden if needed.
(set 'tags (map number->string (iota 9 1)))

;; Define layouts before monitor rules to make sure layout is available
(set-layouts 'tile "[]=" 'dwl:tile)

;; There must be a default monitor rule (i.e. with name == NULL)
(set-monitor-rules '((masters . 1)
                     (master-factor . 0.55)
                     (scale . 1)
                     (rr . TRANSFORM-NORMAL)
                     (layout . tile)))
