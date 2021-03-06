;; system.lsp -- system-dependent lisp code

; local definition for play
;  this one is for Linux:

(setf ny:bigendianp nil)

(if (not (boundp '*default-sf-format*))
    (setf *default-sf-format* snd-head-wave))

(if (not (boundp '*default-sound-file*))
    (setf *default-sound-file* NIL))

(if (not (boundp '*default-sf-dir*))
    (setf *default-sf-dir* "./"))

(if (not (boundp '*default-sf-mode*))
    (setf *default-sf-mode* snd-head-mode-pcm))

(if (not (boundp '*default-sf-bits*))
    (setf *default-sf-bits* 16))

(if (not (boundp '*default-plot-file*))
    (setf *default-plot-file* "points.dat"))


; FULL-NAME-P -- test if file name is a full path or relative path
;
; (otherwise the *default-sf-dir* will be prepended
;
(defun full-name-p (filename)
  (or (eq (char filename 0) #\/)
      (eq (char filename 0) #\.)))


(setf *file-separator* #\/)


;; PLAY-FILE - play a sound file
;;
(defun play-file (name)
;;  (system (strcat "sndplay " (soundfilename name))))
  (system (strcat "play " (soundfilename name) )))

;; R - replay last file written with PLAY
(defun r () (play-file *default-sound-file*))

;;;; use this old version if you want to use sndplay to play
;;;; the result file rather than play the samples as they
;;;; are computed. This version does not autonormalize.
;; PLAY - write value of an expression to file and play it
;;
;(defmacro play (expr)
;  `(prog (specs)
;	 (setf specs (s-save (force-srate *sound-srate* ,expr) 
;			   1000000000 *default-sound-file*))
;	 (r)))
;;;;

; local definition for play
(defmacro play (expr)
  `(s-save-autonorm ,expr NY:ALL *default-sound-file* :play *soundenable*))

;; for Linux, modify s-plot (defined in nyquist.lsp) by saving s-plot
;; in standard-s-plot, then call gnuplot to display the points.
;;
;; we also need to save the location of this file so we can find
;; nyquist-plot.txt, the command file for gnuplot
;;
(setf *runtime-path* (current-path))
(display "system.lsp" *runtime-path*)

(setfn standard-s-plot s-plot)

(defun s-plot (s)
  (let (plot-file)
    (standard-s-plot s) ;; this calculates the data points
    (setf plot-file (strcat *runtime-path* "nyquist-plot.txt"))
    (system (strcat "gnuplot -persist " plot-file))))

