====================================================
Extra Clang Tools 10.0.0 (In-Progress) Release Notes
====================================================

.. contents::
   :local:
   :depth: 3

Written by the `LLVM Team <https://llvm.org/>`_

.. warning::

   These are in-progress notes for the upcoming Extra Clang Tools 10 release.
   Release notes for previous releases can be found on
   `the Download Page <https://releases.llvm.org/download.html>`_.

Introduction
============

This document contains the release notes for the Extra Clang Tools, part of the
Clang release 10.0.0. Here we describe the status of the Extra Clang Tools in
some detail, including major improvements from the previous release and new
feature work. All LLVM releases may be downloaded from the `LLVM releases web
site <https://llvm.org/releases/>`_.

For more information about Clang or LLVM, including information about
the latest release, please see the `Clang Web Site <https://clang.llvm.org>`_ or
the `LLVM Web Site <https://llvm.org>`_.

Note that if you are reading this file from a Subversion checkout or the
main Clang web page, this document applies to the *next* release, not
the current one. To see the release notes for a specific release, please
see the `releases page <https://llvm.org/releases/>`_.

What's New in Extra Clang Tools 10.0.0?
=======================================

Some of the major new features and improvements to Extra Clang Tools are listed
here. Generic improvements to Extra Clang Tools as a whole or to its underlying
infrastructure are described first, followed by tool-specific sections.

Major New Features
------------------

...

Improvements to clangd
----------------------

The improvements are...

Improvements to clang-doc
-------------------------

The improvements are...

Improvements to clang-query
---------------------------

The improvements are...

Improvements to clang-rename
----------------------------

The improvements are...

Improvements to clang-tidy
--------------------------

- New :doc:`linuxkernel-must-use-errs
  <clang-tidy/checks/linuxkernel-must-use-errs>` check.

  Checks Linux kernel code to see if it uses the results from the functions in
  ``linux/err.h``. Also checks to see if code uses the results from functions that
  directly return a value from one of these error functions.

  This is important in the Linux kernel because ``ERR_PTR``, ``PTR_ERR``,
  ``IS_ERR``, ``IS_ERR_OR_NULL``, ``ERR_CAST``, and ``PTR_ERR_OR_ZERO`` return
  values must be checked, since positive pointers and negative error codes are
  being used in the same context. These functions are marked with
  ``__attribute__((warn_unused_result))``, but some kernel versions do not have
  this warning enabled for clang.

- New :doc:`google-upgrade-googletest-case
  <clang-tidy/checks/google-upgrade-googletest-case>` check.

  Finds uses of deprecated Googletest APIs with names containing ``case`` and
  replaces them with equivalent APIs with ``suite``.


Improvements to include-fixer
-----------------------------

The improvements are...

Improvements to clang-include-fixer
-----------------------------------

The improvements are...

Improvements to modularize
--------------------------

The improvements are...

Improvements to pp-trace
------------------------

The improvements are...
