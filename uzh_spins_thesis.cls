\NeedsTeXFormat{LaTeX2e}[1995/12/01]
\ProvidesClass{uzh_spins_thesis}[2026/01/26 v1.0 UZH SPINS LaTeX thesis class]
\LoadClass[10pt,twoside,a4paper,fleqn]{book}
\RequirePackage{uzh_spins}
\RequirePackage{fancyhdr}
\setlength{\textwidth}{15cm}
\setlength{\textheight}{21.3cm}
\setlength{\evensidemargin}{-6mm}
\addtolength{\oddsidemargin}{4mm}
\setlength{\topskip}{0pt}
\setlength{\topsep}{2em}
\setlength{\floatsep}{1.5em plus 2pt minus 2pt}
\setlength{\textfloatsep}{2.5em plus 2pt minus 4pt}
\setlength{\intextsep}{2em}
\setlength{\doublerulesep}{0pt}
\pagestyle{fancy}
\renewcommand{\chaptermark}[1]{\markboth{\chaptername\ \thechapter.\ \ #1}{}}
\renewcommand{\sectionmark}[1]{\markright{\thesection \ \ #1}}
\fancyhf{}
\fancyhead[LE,RO]{\phvbold\small\thepage}
\fancyhead[LO]{\sffamily\rightmark}
\fancyhead[RE]{\sffamily\leftmark}
\renewcommand{\headrulewidth}{0.4pt}
\renewcommand{\footrulewidth}{0pt}
\fancypagestyle{plain}{
  \fancyhead{}
  \renewcommand{\headrulewidth}{0pt}
}
\def\thesisType#1{\gdef\@thesisType{#1}}
\def\subtitle#1{\gdef\@subtitle{#1}}
\def\home#1{\gdef\@home{#1}}
\def\country#1{\gdef\@country{#1}}
\def\legi#1{\gdef\@legi{#1}}
\def\prof#1{\gdef\@prof{#1}}
\def\assistent#1{\gdef\@assistent{#1}}
\def\email#1{\gdef\@email{#1}}
\def\repourl#1{\gdef\@repourl{#1}}
\def\begindate#1{\gdef\@begindate{#1}}
\def\enddate#1{\gdef\@enddate{#1}}
\gdef\@thesisType{No Thesis Type given}
\gdef\@title{No Title given}
\gdef\@subtitle{missing}
\gdef\@date{No Date given}
\gdef\@home{No Home given}
\gdef\@country{No Country given}
\gdef\@legi{No Legi given}
\gdef\@prof{No Professor given}
\gdef\@assistent{}
\gdef\@email{missing}
\gdef\@repourl{missing}
\gdef\@begindate{No Begin Date given}
\gdef\@enddate{No End Date given}
\def\missing{missing}
\newif\ifsubtitle
\subtitletrue
\newif\ifemail
\emailtrue
\newif\ifrepourl
\repourltrue
\newlength{\normaltopmargin}
\setlength\normaltopmargin{\topmargin}
\renewcommand\maketitle{%
  \ifx\@subtitle\missing
    \subtitlefalse
  \fi
  \ifx\@email\missing
    \emailfalse
  \fi
  \ifx\@repourl\missing
    \repourlfalse
  \fi
  \newpage
  \thispagestyle{empty}
  \begingroup
  \fontfamily{phv}\selectfont
  \begin{flushright}%
    \vspace*{50\p@}
    {\fontsize{20}{0}\selectfont \@thesisType}
    \vskip -9\p@
    \vbox{\hrule height0.25pt width\textwidth}
    \vskip -2\p@
    {\fontsize{9}{0}\selectfont \@date}
    \vskip 20\p@
    {\fontfamily{phv}\fontsize{32}{34}\selectfont\bfseries \@title\par}
    \ifsubtitle
      \vskip -12\p@  
	\parbox{\textwidth}{
	\begin{flushright}
		\fontfamily{phv}\fontsize{20}{25}\selectfont \@subtitle
	\end{flushright}}
    \fi
    \vfill
    {\fontsize{20}{0}\selectfont\bfseries \@author}
    \vskip 2\p@
    {\fontsize{8}{0}\selectfont of \@home,\ \@country \ (\@legi)}
    \vskip 35\p@
    {\fontsize{10}{0}\selectfont\bfseries supervised by}
    \vskip 4\p@
    {\fontsize{11}{0}\selectfont \@prof}
 \end{flushright}%
 \vskip 87\p@
 \parindent \z@
 \includegraphics[width=4.5cm]{logo_uzh}
 \endgroup
 \clearpage\thispagestyle{empty}\cleardoublepage
  \thispagestyle{empty}
  \begingroup
  \fontfamily{phv}\selectfont
  \begin{flushright}%
    \vspace*{50\p@}
    {\fontsize{20}{0}\selectfont \@thesisType}
    \vskip -9\p@
    \vbox{\hrule height0.25pt width\textwidth}
    \vskip 31\p@
    {\fontsize{32}{34}\selectfont\bfseries \@title\par}
    \ifsubtitle
     \vskip -12\p@  
	\parbox{\textwidth}{
	\begin{flushright}
		\fontfamily{phv}\fontsize{20}{25}\selectfont \@subtitle
	\end{flushright}}
    \fi
    \vfill
    {\fontsize{20}{0}\selectfont\bfseries \@author}
 \end{flushright}%
 \vskip 174\p@
 \parindent \z@
 \includegraphics[width=4.5cm]{logo_uzh}
 \endgroup
 \clearpage\thispagestyle{empty}
 \normalfont
 \vspace*{\fill}
 \noindent
 \hspace{-0.9em}
 \begin{tabular}{ll}
   {\bf \@thesisType} & \\[4\p@]
   {\bf Author:} & \@author \ifemail , \@email\fi\\[4\p@]
   \ifrepourl
   {\bf URL:} & \@repourl \\[4\p@]
   \fi
   {\bf Project period:} & \@begindate \ - \@enddate \\[4\p@]
 \end{tabular}
 \vskip 4\p@
 \noindent Security and Privacy of Information, Networks, and Systems (SPINS)\\[2\p@]
 \noindent Department of Informatics (IfI), University of Zurich%
}
\renewcommand\chapter{\if@openright
                        \clearpage\thispagestyle{empty}\cleardoublepage
                    \else
                        \clearpage
                    \fi
                    \thispagestyle{empty}%
                    \global\@topnum\z@
                    \@afterindentfalse
                    \secdef\@chapter\@schapter}
\def\@chapter[#1]#2{\ifnum \c@secnumdepth >\m@ne
                       \if@mainmatter
                         \refstepcounter{chapter}%
                         \typeout{\@chapapp\space\thechapter.}%
                         \addcontentsline{toc}{chapter}%
                                   {\protect\numberline{\thechapter}#1}%
                       \else
                         \addcontentsline{toc}{chapter}{#1}%
                       \fi
                    \else
                      \addcontentsline{toc}{chapter}{#1}%
                    \fi
                    \chaptermark{#1}%
                    \addtocontents{lof}{\protect\addvspace{10\p@}}%
                    \addtocontents{lot}{\protect\addvspace{10\p@}}%
                    \if@twocolumn
                      \@topnewpage[\@makechapterhead{#2}]%
                    \else
                      \@makechapterhead{#2}%
                      \@afterheading
                    \fi}
\def\@makechapterhead#1{%
   \vspace*{20\p@}%
   {\parindent \z@ \raggedright
     \interlinepenalty\@M
     \ifnum \c@secnumdepth >\m@ne
       \if@mainmatter
         {\fontfamily{phv}\fontsize{20}{24}\selectfont\bfseries \thechapter.\ \ #1\par}
       \else
         {\fontfamily{phv}\fontsize{20}{24}\selectfont\bfseries #1\par}
       \fi
     \else
       {\fontfamily{phv}\fontsize{20}{24}\selectfont\bfseries #1\par}
     \fi
     \vskip 20\p@
   }}
 \def\@schapter#1{\if@twocolumn
                    \@topnewpage[\@makeschapterhead{#1}]%
                  \else
                    \@makeschapterhead{#1}%
                    \@afterheading
                  \fi}
 \def\@makeschapterhead#1{%
   \vspace*{20\p@}%
   {\parindent \z@ \raggedright
     \interlinepenalty\@M
     {\fontfamily{phv}\fontsize{20}{24}\selectfont\bfseries #1\par}
     \vskip 20\p@
   }}
\let\origdoublepage\cleardoublepage
\renewcommand{\cleardoublepage}{%
  \clearpage
  {\pagestyle{empty}\origdoublepage}%
}
\renewcommand\section{\@startsection {section}{1}{\z@}%
                                   {-3.8ex \@plus -1ex \@minus -.2ex}%
                                   {2.3ex \@plus.2ex}%
                                   {\fontfamily{phv}\fontsize{13}{16}\selectfont\raggedright}}
\renewcommand\subsection{\@startsection{subsection}{2}{\z@}%
                                     {-3.3ex\@plus -1ex \@minus -.2ex}%
                                     {1.5ex \@plus .2ex}%
                                     {\fontfamily{phv}\fontsize{11}{14}\selectfont\raggedright}}
\renewcommand\subsubsection{\@startsection{subsubsection}{3}{\z@}%
                                     {-3.3ex\@plus -1ex \@minus -.2ex}%
                                     {1.5ex \@plus .2ex}%
                                     {\fontfamily{phv}\fontsize{10}{12}\selectfont\raggedright}}
\renewcommand\paragraph{\@startsection{paragraph}{4}{\z@}%
                                    {3.25ex \@plus1ex \@minus.2ex}%
                                    {-1em}%
                                    {\fontfamily{phv}\fontsize{10}{12}\selectfont\raggedright}}
% regla inferior tras títulos de secciones
\let\orig@section\section
\let\orig@subsection\subsection
\let\orig@subsubsection\subsubsection
\let\orig@paragraph\paragraph
\renewcommand{\section}[1]{\orig@section{#1}\par\nobreak\vskip 12\p@}
\renewcommand{\subsection}[1]{\orig@subsection{#1}\par\nobreak\vskip 10\p@}
\renewcommand{\subsubsection}[1]{\orig@subsubsection{#1}\par\nobreak\vskip 8\p@}
\renewcommand{\paragraph}[1]{\orig@paragraph{#1}\par\nobreak\vskip 6\p@}
\renewcommand\tableofcontents{%
	\if@twocolumn
		\@restonecoltrue\onecolumn
	\else
		\@restonecolfalse
	\fi
	\chapter*{\contentsname
		\@mkboth{%
			\contentsname}{\contentsname}}%
	\@starttoc{toc}%
	\if@restonecol\twocolumn\fi
}
\renewcommand\listoffigures{%
    \newpage
    \section*{\listfigurename}
        \@starttoc{lof}%
}
\renewcommand\listoftables{%
    \section*{\listtablename}
        \@starttoc{lot}%
}
\renewcommand\lstlistoflistings{%
    \section*{List of \lstlistlistingname}
        \@starttoc{lol}%
}
\renewcommand\bibname{References}
\newenvironment{abstract}{%
  \chapter*{Abstract}}
  {}
\newenvironment{zusammenfassung}{%
  \chapter*{Zusammenfassung}}{}
\newenvironment{acknowledgements}{%
  \chapter*{Acknowledgements}}{}
\endinput
