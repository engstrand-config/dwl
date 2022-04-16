(use-modules (dwl-guile packages)
             (guix gexp)
             (guix utils)
             (guix packages)
             (guix download)
             (guix git-download)
             (gnu packages wm)
             (gnu packages guile)
             (gnu packages xdisorg)
             (gnu packages libffi)
             (gnu packages freedesktop))

(define this-directory
  (dirname (current-filename)))

(define source
  (local-file this-directory
              #:recursive? #t
              #:select? (git-predicate this-directory)))

(define libdrm-2.4.109
  (package
   (inherit libdrm)
   (name "libdrm")
   (version "2.4.109")
   (source (origin
            (method url-fetch)
            (uri (string-append
                  "https://dri.freedesktop.org/libdrm/libdrm-"
                  version ".tar.xz"))
            (sha256
             (base32
              "09kzrdsd14zr0i3izvi5mck4vqccl3c9hr84r9i4is0zikh554v2"))))))

(define wayland-1.20.0
  (package
   (inherit wayland)
   (name "wayland")
   (version "1.20.0")
   (source (origin
            (method url-fetch)
            (uri (string-append "https://wayland.freedesktop.org/releases/"
                                name "-" version ".tar.xz"))
            (sha256
             (base32
              "09c7rpbwavjg4y16mrfa57gk5ix6rnzpvlnv1wp7fnbh9hak985q"))))
   (propagated-inputs
    (list libffi))))

(define wayland-protocols-1.24
  (package
   (inherit wayland-protocols)
   (name "wayland-protocols")
   (version "1.24")
   (source (origin
            (method url-fetch)
            (uri (string-append
                  "https://wayland.freedesktop.org/releases/"
                  "wayland-protocols-" version ".tar.xz"))
            (sha256
             (base32
              "1hlb6gvyqlmsdkv5179ccj07p04cn6xacjkgklakbszczv7xiw5z"))))
   (inputs
    (list wayland-1.20.0))))

(define wlroots-0.15.0
  (package
   (inherit wlroots)
   (name "wlroots")
   (version "0.15.0")
   (source
    (origin
     (method git-fetch)
     (uri (git-reference
           (url "https://gitlab.freedesktop.org/wlroots/wlroots")
           (commit version)))
     (file-name (git-file-name name version))
     (sha256
      (base32 "0wdzs0wpv61pxgy3mx3xjsndyfmbj30v47d3w9ymmnd4r479n41n"))))
   (propagated-inputs
    (modify-inputs (package-propagated-inputs wlroots)
                   (prepend libdrm-2.4.109)
                   (replace "wayland-protocols" wayland-protocols-1.24)
                   (replace "wayland" wayland-1.20.0)))))

(package
  (inherit dwl)
  (source source)
  (name "dwl-guile-devel")
  (inputs
    `(("guile-3.0" ,guile-3.0)
      ("wlroots-0.15.0" ,wlroots-0.15.0)))
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
