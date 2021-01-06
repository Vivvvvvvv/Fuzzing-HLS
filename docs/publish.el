;; Publishing projects, this one is for the zettelkasten
(require 'package)
(package-initialize)
(add-to-list 'package-archives '("org" . "https://orgmode.org/elpa/") t)
(add-to-list 'package-archives '("melpa" . "https://melpa.org/packages/") t)
(package-refresh-contents)
(package-install 'org-plus-contrib)
(package-install 'htmlize)
(package-install 'solarized-theme)

(require 'org)
(require 'ox-publish)

(load-theme 'solarized-light t)
(defvar fuzzing-hls/header "")
(defvar fuzzing-hls/site-attachments nil)
(defvar fuzzing-hls/base "")

(setq fuzzing-hls/base "/fuzzing-hls")

(setq fuzzing-hls/site-attachments
  (regexp-opt '("jpg" "jpeg" "gif" "png" "svg"
                "ico" "cur" "css" "js" "woff" "woff2" "ttf"
                "html" "pdf")))

(setq user-full-name nil)

(setq org-export-with-smart-quotes t
      org-export-with-section-numbers t
      org-export-with-toc t)

(setq org-html-divs '((preamble "header" "header")
                      (content "article" "content")
                      (postamble "footer" "postamble"))
      org-html-container-element "section"
      org-html-metadata-timestamp-format "%Y-%m-%d"
      org-html-checkbox-type 'html
      org-html-html5-fancy t
      org-html-validation-link nil
      org-html-doctype "html5"
      org-html-coding-system 'utf-8-unix
      org-html-head-include-default-style nil
      org-html-head-include-scripts nil)

(setq org-publish-project-alist
      (list
       (list "fuzzing-hls-org"
             :base-directory "./"
             :base-extension "org"
             :exclude (regexp-opt '("README" "draft"))
             :html-head-extra
             (concat "<link rel=\"stylesheet\" href=\"" fuzzing-hls/base "/css/org.css\" type=\"text/css\" media=\"screen\" />")
             :publishing-directory "./html"
             :publishing-function 'org-html-publish-to-html
             :recursive t)
       (list "fuzzing-hls-assets"
             :base-directory "."
             :base-extension fuzzing-hls/site-attachments
             :include '(".nojekyll")
             :exclude "html/"
             :publishing-directory "./html"
             :publishing-function 'org-publish-attachment
             :recursive t)
       (list "fuzzing-hls" :components '("fuzzing-hls-org" "fuzzing-hls-assets"))))

(defun publish-fuzzing-hls-docs ()
  "Publish Fuzzing-Hls documentation."
  (interactive)
  (org-publish "fuzzing-hls" t))
