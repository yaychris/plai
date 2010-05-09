;; The first three lines of this file were inserted by DrScheme. They record metadata
;; about the language level of this file in a form that our tools can easily process.
#reader(planet plai/plai:1:20/lang/reader)
(define-type AE
  (num (n number?))
  (add (lhs AE?)
       (rhs AE?))
  (sub (lhs AE?)
       (rhs AE?)))

(define (parse sexp)
  (cond
    ((number? sexp) (num sexp))
    ((list? sexp)
     (case (first sexp)
       ((+) (add (parse (second sexp))
                 (parse (third sexp))))
       ((-) (sub (parse (second sexp))
                 (parse (third sexp))))))))

(define (calc an-ae)
  (type-case AE an-ae
    (num (n) n)
    (add (l r) (+ (calc l) (calc r)))
    (sub (l r) (- (calc l) (calc r)))))
