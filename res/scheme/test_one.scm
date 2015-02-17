;; let's just test
(write "Hello world")
(newline)
;; test image api
(define image-name-list (image-names))
;;

(write "Images")
(newline)
(write "======")
(newline)
(write image-name-list)
(newline)
(define image-file-list (image-file-names))
(write "Image Files")
(newline)
(write "======")
(newline)
(write image-file-list)
(newline)
