;; The first three lines of this file were inserted by DrScheme. They record metadata
;; about the language level of this file in a form that our tools can easily process.
#reader(planet plai/plai:1:20/lang/reader)
(define-type FAE
  (num (n number?))
  (add (lhs FAE?) (rhs FAE?))
  (id (name symbol?))
  (fun (param symbol?) (body FAE?))
  (app (fun-expr FAE?) (arg-expr FAE?)))

(define-type FAE-Value
  (numV (n number?))
  (closureV (param symbol?)
            (body FAE?)
            (ds DefrdSub?)))

(define-type DefrdSub
  (mtSub)
  (aSub (name symbol?) (value FAE-Value?) (ds DefrdSub?)))

(define (lookup name ds)
  (type-case DefrdSub ds
    (mtSub () (error 'lookup "no binding for indentifier"))
    (aSub (bound-name bound-value rest-ds)
          (if (symbol=? bound-name name)
              bound-value
              (lookup name rest-ds)))))

(define (parse sexp)
  (cond
    ((number? sexp) (num sexp))
    ((symbol? sexp) (id sexp))
    ((list? sexp)
     (case (first sexp)
       ((with) (app
                (fun (first (second sexp))
                     (parse (third sexp)))
                (parse (second (second sexp)))))
       ((fun) (fun (first (second sexp))
                   (parse (third sexp))))
       ((+) (add (parse (second sexp))
                 (parse (third sexp))))
       (else (app (parse (first sexp))
                  (parse (second sexp))))))))

(define (num+ l r)
  (numV (+ (numV-n l) (numV-n r))))

(define (interp expr ds)
  (type-case FAE expr
    (num (n) (numV n))
    (add (l r) (num+ (interp l ds) (interp r ds)))
    (id (v) (lookup v ds))
    (fun (bound-id bound-body)
         (closureV bound-id bound-body ds))
    (app (fun-expr arg-expr)
         (local ((define fun-val (interp fun-expr ds)))
           (interp (closureV-body fun-val)
                   (aSub (closureV-param fun-val)
                         (interp arg-expr ds)
                         (closureV-ds fun-val)))))))