(defvar mytree-mode-map (make-sparse-keymap) "")
(defvar mytree-indent-string "  ")
(defvar mytree-dir-regex nil)
(defvar mytree-inhibit-dir-regex nil)
(defvar mytree-file-regex nil)
(defvar mytree-inhibit-file-regex nil)

(setq mytree-inhibit-dir-regex "^CVS$\\|^obj-.*")
(setq mytree-file-regex "\\.[cChHsS]$")
(setq mytree-inhibit-file-regex "^\\.")

(defun mytree-insert-subdirs (prefix dirlist)
  (while dirlist 
    (insert prefix ">" (car dirlist) "\n")
    (setq dirlist (cdr dirlist))))

(defun mytree-count-depth (limit) 
  (let ((count 0))
    (save-excursion
      (beginning-of-line)
      (while (search-forward mytree-indent-string limit t)
	(setq count (+ count 1))))
      count))
    
(defun mytree-parse-current-line ()
  (let (info begin end statepos)
    (save-excursion
      (end-of-line)
      (setq end (point))
      (beginning-of-line)
      (setq begin (point))
      (if (re-search-forward (concat "^\\(" mytree-indent-string "\\)*[\\>\\< ]") end t)
;      (if (re-search-forward "[\>\<]" end t)
	  (progn (setq  statepos (point))
	         (setq  info (cons (mytree-count-depth statepos) '()))
		 (setq  info (append info (list (preceding-char))))
		 (setq  info (append info (list (buffer-string statepos end))))
		 (setq  info (append info (list statepos)))
		 info
		 )
	nil)
)))

(defun mytree-build-path (currentdir currentdepth)
  (let ((path currentdir) info)
    (save-excursion
      (beginning-of-line)
      (while (> currentdepth 1)
	(setq currentdepth (- currentdepth 1))
	(re-search-backward (concat "^" (mytree-create-prefix currentdepth) "[\\>\\<]") nil t)
	(setq info (mytree-parse-current-line))
	(if info 
	    (setq path (concat (file-name-as-directory (car(cdr(cdr info)))) path))
	  )
	))
      (if (= currentdepth 0)
	  (file-name-as-directory root)
	(concat (file-name-as-directory root) path)))
)

(defun mytree-toggle-thisdir ()
  (interactive)
  (let ((buffer-read-only nil)
	info
	depth)
    (save-excursion
      (setq info (mytree-parse-current-line))
      (setq depth (car info))
      (if info
	  (progn (setq info (cdr info))
	         (forward-line 1)
		 (if (char-equal (car info) ?>)
		     (progn 
		       (setq info (cdr info))
		       (mytree-insert-tree  
			(mytree-build-path (car info) depth) 
			(+ depth 1))
		       (setq info (cdr info))
		       (goto-char (car info))
		       (delete-char -1)
		       (insert-char ?<)
		       )
		   (if (char-equal (car info) ?<)
		       (progn (setq info (cdr (cdr info)))
			      (mytree-delete-tree depth)
			      (goto-char (car info))
			      (delete-char -1)
			      (insert-char ?>)
		   )))
		 )))))

(defun mytree-thisfile ()
  (let ((buffer-read-only nil)
	info
	depth)
    (save-excursion
      (setq info (mytree-parse-current-line))
      (setq depth (car info))
      (if info
	  (progn
	    (setq info (cdr (cdr info)))
	    (mytree-build-path (car info) depth)
)))))

(defun mytree-edit-this ()
  (interactive)
  (find-file-other-window (mytree-thisfile)))

(defun mytree-files-present (depth)
  (let ((prefix (mytree-create-prefix depth))
	end)
    (save-excursion
      (end-of-line)
      (setq end (point))
      (beginning-of-line) 
      (re-search-forward (concat "^" prefix " .+") end t)
       )))

(defun mytree-dirs-present (depth)
  (let ((prefix (mytree-create-prefix depth))
	end)
    (save-excursion
      (end-of-line)
      (setq end (point))
      (beginning-of-line) 
      (re-search-forward (concat "^" prefix "[\\>\\<]") end t)
       )))

(defun mytree-list-files-thisdir ()
  (interactive)
  (let ((buffer-read-only nil)
	info
	depth)
    (save-excursion
      (setq info (mytree-parse-current-line))
      (setq depth (car info))
      (setq info (cdr info))
      (if info
	  (if (or (char-equal (car info) ?>)
		 (char-equal (car info) ?<))
	      (progn
		(if (char-equal (car info) ?>)
		    (mytree-toggle-thisdir))
		(forward-line 1)
		(if (mytree-files-present (+ depth 1))
		    (progn 
		      (mytree-delete-tree (+ depth 1))
		      (forward-line 1)
		      (if (not (mytree-dirs-present (+ depth 1)))
			  (progn 
			    (setq info (cdr (cdr info)))
			    (goto-char (car info))
			    (delete-char -1)
			    (insert-char ?>)
			    )))
		  (progn 
		    (setq info (cdr info))
		    (mytree-insert-files (mytree-build-path (car info) depth) (+ depth 1))
	    ))))))))

(defun mytree-list-dir (dir) 
  (let (subdirlist
	subdirname
	(tmplist (directory-files dir nil mytree-dir-regex nil 'd )))
    (while tmplist
      (setq subdirname (car tmplist))
      (if (not (or (string-match "^\\.\\.?$" subdirname)
		   (and mytree-inhibit-dir-regex
			(string-match mytree-inhibit-dir-regex
				      subdirname))))
	  (setq subdirlist (append subdirlist (list subdirname))))
      (setq tmplist (cdr tmplist))
      )
    subdirlist
    )
)

(defun mytree-deep-toggle-thisdir ()
  (interactive)
  (save-excursion
    (mytree-depth-iterate 'mytree-toggle-thisdir)
))

(defun mytree-deep-list-files-thisdir ()
  (interactive)
  (save-excursion
    (mytree-depth-iterate 'mytree-list-files-thisdir)
))

(defun mytree-depth-iterate (f)
  (let ((info (mytree-parse-current-line))
	depth
	)
    (if (not (null info))
	(progn (setq depth (car info))
	       (funcall f)
	       (while (and (= (forward-line 1) 0)
			   (progn 
			     (setq info (mytree-parse-current-line))
			     (if (not (null info))
				 (> (car info) depth))))
		 (funcall f))))))
      
(defun mytree-list-files (dir)
  (let (filelist
	filename
	(tmplist (directory-files dir nil mytree-file-regex nil t )))
    (while tmplist
      (setq filename (car tmplist))
      (if (not (and mytree-inhibit-file-regex
		    (string-match mytree-inhibit-file-regex
				  filename)))
	  (setq filelist (append filelist (list filename)))
	)
      (setq tmplist (cdr tmplist))
      )
    filelist
    )
  )


(defun mytree-create-prefix (depth) 
  (let ((result ""))
    (while (> depth 0)
      (setq result (concat result mytree-indent-string))
      (setq depth (- depth 1)))
    result))

(defun mytree-insert-tree (dir depth)
  (let ((buffer-read-only nil)
	(subdirlist (mytree-list-dir dir)))
    (save-excursion
      (mytree-insert-subdirs (mytree-create-prefix depth) subdirlist))))

(defun mytree-insert-files (dir depth)
  (let ((buffer-read-only nil)
	(filelist (mytree-list-files dir))
	(prefix (mytree-create-prefix depth)))
    (save-excursion
      (while filelist 
	(insert prefix " " (car filelist) "\n")
	(setq filelist (cdr filelist))))
))

(defun mytree-delete-tree (depth)
  (let ((buffer-read-only nil) 
	info
	d
	begin)
    (save-excursion
      (setq begin (point))

      (if (= depth 0)
	  (goto-char (point-max))
	(progn 
	  (re-search-forward (concat "^\\(" mytree-indent-string "\\)*[\\>\\<]") nil 1)
	  (setq info (mytree-parse-current-line))
	  (if (null info)
	      (goto-char (point-max))
	    (progn 
	      (setq d (car info))
	      (while  (and  
		       info
		       (> d depth)
		       (re-search-forward (concat "^\\(" mytree-indent-string "\\)*[\\>\\<]") nil 1))
		(setq info (mytree-parse-current-line))
		(if (null info)
		    (goto-char (point-max))
		  (setq d (car info))
		  ))
	      ))))


      (if (> (count-lines begin (point))  0)
	  (progn (beginning-of-line)
		 (delete-region begin (point))))
)))
	
(defun mytree-insert-root (dir) 
  (let ((buffer-read-only nil))
    (setq root dir)
    (insert "<" (abbreviate-file-name (file-name-as-directory dir)) "\n")
    (mytree-insert-tree dir 1)))

(defun  mytree (filename)
  (interactive "DDirectory: ")
  (save-excursion
    ( let ((dirname (file-name-as-directory
		     (abbreviate-file-name (expand-file-name filename))))
	   (buffer nil))
      (setq buffer (create-file-buffer (directory-file-name dirname)))
      (set-buffer buffer)
      (mytree-mode)
      (cd filename)
      (mytree-insert-root dirname)
      (set-window-buffer (selected-window) buffer)))
)

(defun mytree-set-root (dir)
  (interactive "DDirectory: ")
  (let ((buffer-read-only nil)
	(begin (point-min)))
    (if (file-directory-p dir) 
	(progn 
	  (goto-char (point-max))
	  (delete-region begin (point))
	  (cd dir)
	  (mytree-insert-root dir)))))
  
(defun mytree-set-root-this ()
  (interactive)
  (mytree-set-root (mytree-thisfile)))

(defun mytree-up-root ()
  (interactive)
  (mytree-set-root (expand-file-name ".." root)))

(suppress-keymap mytree-mode-map)

(defun mytree-keymap-define ( map binding-list )
  (mapcar (lambda ( binding )
	       (define-key map (car binding) (cdr binding)))
	  binding-list))

(mytree-keymap-define
 mytree-mode-map
 '(
  ;; Showing and hiding sublevels
   ("i" 	. mytree-toggle-thisdir)
   ("I" 	. mytree-deep-toggle-thisdir)
   ("f" 	. mytree-list-files-thisdir)
   ("F" 	. mytree-deep-list-files-thisdir)
   ("e"         . mytree-edit-this)
   ("r"         . mytree-set-root-this)
   ("u"         . mytree-up-root)
   ("p"		. previous-line)
   ("n"		. next-line)
   ))



(defun mytree-mode ()
  (interactive)
  (kill-all-local-variables)
  (use-local-map mytree-mode-map)
  (setq mode-name "MyTreeMode")
  (setq major-mode 'mytree-mode)
  (setq local-abbrev-table nil)
;  (set-syntax-table mytree-table)
  (setq indent-tabs-mode nil)
  (auto-fill-mode -1)
  (buffer-disable-undo (current-buffer))
;  (setq case-fold-search nil)
  (setq buffer-read-only t)
;; Local Variables
  (set (make-local-variable 'root) nil)
)