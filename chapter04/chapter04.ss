;; The first three lines of this file were inserted by DrScheme. They record metadata
;; about the language level of this file in a form that our tools can easily process.
#reader(planet plai/plai:1:20/lang/reader)
(define-type F1WAE
  (num (n number?))
  (add (lhs F1WAE?)
       (rhs F1WAE?))
  (with (name symbol?) (named-expr F1WAE?) (body F1WAE?))
  (id (name symbol?))
  (app (fun-name symbol?) (arg F1WAE?)))

(define-type FunDef
  (fundef (fun-name symbol?)
          (arg-name symbol?)
          (body F1WAE?)))

(define (parse sexp)
  (cond
    ((number? sexp) (num sexp))
    ((symbol? sexp) (id sexp))
    ((list? sexp)
     (case (first sexp)
       ((with) (with (first (second sexp))
                     (parse (second (second sexp)))
                     (parse (third sexp))))
       ((+) (add (parse (second sexp))
                 (parse (third sexp))))
       (else (app (first sexp)
                  (parse (second sexp))))))))

(define (lookup-fundef fun-name fundefs)
  (cond
    ((empty? fundefs) (error fun-name "function not found"))
    (else (if (symbol=? fun-name (fundef-fun-name (first fundefs)))
              (first fundefs)
              (lookup-fundef fun-name (rest fundefs))))))

(define (subst expr sub-id val)
  (type-case F1WAE expr
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
    (app (fun-name arg-expr)
         (app fun-name (subst arg-expr sub-id val)))))


(define (interp expr fun-defs)
  (type-case F1WAE expr
    (num (n) n)
    (add (l r) (+ (interp l fun-defs) (interp r fun-defs)))
    (with (bound-id named-expr bound-body)
          (interp (subst bound-body
                         bound-id
                         (num (interp named-expr fun-defs)))
                  fun-defs))
    (id (v) (error 'interp "free identifier"))
    (app (fun-name arg-expr)
         (local ((define the-fun-def (lookup-fundef fun-name fun-defs)))
           (interp (subst (fundef-body the-fun-def)
                          (fundef-arg-name the-fun-def)
                          (num (interp arg-expr fun-defs)))
                   fun-defs)))))