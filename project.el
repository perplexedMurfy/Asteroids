(setq project-dir default-directory)

(setq compile-command (concat project-dir "build.bat "))
(setq run-command (concat project-dir "run.bat"))

(defun compile-project ()
  (interactive)
  "Defined in project.el
Compiles the project we're working on."
  (compile compile-command))

(defun run-project ()
  (interactive)
  "Defined in project.el
Runs the project that we're working on."
  (make-process :name "project-run" :buffer "*run*" :command (list run-command)))

(define-key murfy-keys-mode-map (kbd "<M-f1>") 'compile-project)
(define-key murfy-keys-mode-map (kbd "<f2>") 'run-project)

(find-file "./project.el" t)
(find-file ".gitignore" t)
(find-file "./src/*.*" t)
