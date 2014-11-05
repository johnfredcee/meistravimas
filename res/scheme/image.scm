; Scheme primitives for working with images

(define (message selector the-instance . args)
  (apply (the-instance selector) args)

(define (make-image-list channels size)
  (let next-channel ((channel channels)
					 (result '()))
	(if (null? channel) 
		result
		(next-channel (cdr channel) (cons (cons (car channel) (make-vector size)) result)))))
  
(define (make-image width height channels)
  (let ((data (make-image-list channels (* height width))))
	(lambda (selector)
	  (cond ((eq? selector 'width)
			 (lambda () width))
			((eq? selector 'height)
			 (lambda () height))
			((eq? selector 'channels)
			 (lambda () channels))
			((eq? selector 'channel)
			 (lambda (c) (assq c channels)))
			((eq? selector 'get-pixel)
			 (lambda (x y)
			   (let ((index (+ (* y width) x)))
				 (map (lambda (c) (vector-ref (cadr c) index)) channels))))
			((eq? selector 'set-pixel)
			 (lambda (x y . vals)
			   (let ((index (+ (* y width) x)))			   
				 (map (lambda (c v) (vector-set! (cadr c) index (cadr v))) channels vals))))))))
				  
			 




