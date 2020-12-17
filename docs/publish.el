;; Publishing projects, this one is for the zettelkasten
(require 'package)
(package-initialize)
(add-to-list 'package-archives '("org" . "https://orgmode.org/elpa/") t)
(add-to-list 'package-archives '("melpa" . "https://melpa.org/packages/") t)
(package-refresh-contents)
(package-install 'org-plus-contrib)
(package-install 'htmlize)

(require 'org)
(require 'ox-publish)

(defvar vericert/header "")
(defvar vericert/site-attachments nil)
(defvar vericert/base "")

(setq vericert/base "/fuzzing-hls")

(setq vericert/site-attachments
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
       (list "vericert-org"
             :base-directory "./"
             :base-extension "org"
             :exclude (regexp-opt '("README" "draft"))
             :html-head-extra
             (concat "<link rel=\"stylesheet\" href=\"" vericert/base "/css/org.css\" type=\"text/css\" media=\"screen\" />")
             :publishing-directory "./html"
             :publishing-function 'org-html-publish-to-html
             :recursive t)
       (list "vericert-assets"
             :base-directory "."
             :base-extension vericert/site-attachments
             :include '(".nojekyll")
             :exclude "html/"
             :publishing-directory "./html"
             :publishing-function 'org-publish-attachment
             :recursive t)
       (list "vericert" :components '("vericert-org" "vericert-assets"))))

(defun publish-vericert-docs ()
  "Publish Vericert documentation."
  (interactive)
  (org-publish "vericert" t))
