;; The first three lines of this file were inserted by DrScheme. They record metadata
;; about the language level of this file in a form that our tools can easily process.
#reader(planet plai/plai:1:20/lang/reader)
(define-type FWAE
  (num (n number?))
  (add (lhs FWAE?)
       (rhs FWAE?))
  (with (name symbol?) (named-expr FWAE?) (body FWAE?))
  (id (name symbol?))
  (fun (param symbol?) (body FWAE?))
  (app (fun-expr FWAE?) (arg-expr FWAE?)))

(define (parse sexp)
  (cond
    ((number? sexp) (num sexp))
    ((symbol? sexp) (id sexp))
    ((list? sexp)
     (case (first sexp)
       ((with) (with (first (second sexp))
                     (parse (second (second sexp)))
                     (parse (third sexp))))
       ((fun) (fun (first (second sexp))
                   (parse (third sexp))))
       ((app) (app (parse (second sexp))
                   (parse (third sexp))))
       ((+) (add (parse (second sexp))
                 (parse (third sexp))))))))

(define (subst expr sub-id val)
  (type-case FWAE expr
    (num (n) expr)
    (add (l r) (add (subst l sub-id val)
                    (subst r sub-id val)))
    (with (bound-id named-expr bound-body)
          (if (symbol=? bound-id sub-id)
              (with bound-id
                    (subst named-expr sub-id val)
                    bound-body)
              (with bound-id
                    (subst named-expr sub-id val)
                    (subst bound-body sub-id val))))
    (id (v) (if (symbol=? v sub-id) val expr))
    (else #f)))

(define (add-numbers l r)
  (+ (num-n l) (num-n r)))

(define (interp expr)
  (type-case FWAE expr
    (num (n) n)
    (add (l r) (add-numbers (interp l) (interp r)))
    (with (bound-id named-expr bound-body)
          (interp (subst bound-body
                         bound-id
                         (interp named-expr))))
    (id (v) (error 'calc "free identifier"))
    (fun (bound-id bound-body) expr)
    (app (fun-expr arg-expr)
         (local ((define fun-val (interp fun-expr)))
           (interp (subst (fun-body fun-val)
                          (fun-param fun-val)
                          (interp arg-expr)))))))