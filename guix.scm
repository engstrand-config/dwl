(use-modules (dwl-guile packages)
             (guix gexp)
             (guix utils)
             (guix packages)
             (guix git-download)
             (gnu packages wm)
             (gnu packages guile))

(define this-directory
  (dirname (current-filename)))

(define source
  (local-file this-directory
              #:recursive? #t
              #:select? (git-predicate this-directory)))

(package
  (inherit dwl)
  (source source)
  (name "dwl-guile-devel")
  (inputs
    `(("guile-3.0" ,guile-3.0)
      ("wlroots" ,wlroots)))
  (arguments
    (substitute-keyword-arguments
      (package-arguments dwl)
      ((#:phases phases)
       `(modify-phases
          ,phases
            (replace
              'install
              (lambda*
                (#:key inputs outputs #:allow-other-keys)
                (let ((bin (string-append (assoc-ref outputs "out") "/bin")))
                  (install-file "dwl" bin)
                  (rename-file (string-append bin "/dwl")
                               (string-append bin "/dwl-guile-devel"))
                  #t))))))))
