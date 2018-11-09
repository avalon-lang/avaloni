# Contributing to Avalon

Thanks for considering contributing to Avalon. This document describes where and how to contribute.
If you are new to Avalon, please first read the documentation available at [Read The Docs](https://avalon-lang.readthedocs.io/en/latest).

## Code of conduct

When submitting your patch, please make sure to avoid rude, offensive and/or hate-inciting language.
This is applicable to your comments in the code, commits and issues.

We seek to form a diverse and inclusive community and people who are seeking to contribute come from all backgrounds 
and we cannot assume them to know everything that seem familiar to us.

Let us take make special of women, transgender and minorities who are often harassed online as well as offline.
Any behavior that make anyone feel unwelcome will result in an immediate banned from the community after exactly one warning, no more and no less.
All infractions will be publicly recorded for future reference before the ban.

The language is designed under the mantra of no case is special enough to merit exceptional treatement and 
the same mantra will be applied to every contributor: you are not special enough to warrant special treatement 
that drives others away.

## Learning Avalon

The [documentation](https://avalon-lang.readthedocs.io/en/latest) provides tutorials and the language reference 
and is a good place to start if you want to learn Avalon.

## Filing an issue

As the language is still in version 0.0.1, feel free to file any issue that you feel will help improve the language.
This includes but not limited to: bug reports, feature requests and documentation clarification and expansion.

## Where to contribute

Before contributing, please make sure to have all items on the contributor checklist ticked.

### Contributor checklist

* You must have a Github account. If you don't, please create one using [Github sign up](https://github.com/signup/free).

* [Fork Avalon](https://github.com/avalon-lang/avalon/fork).

* Install the compiler. Instructions are available at [Installing Avalon](https://avalon-lang.readthedocs.io/en/latest/installation.html).

* Make sure you know Git.

* Start hacking.

* Send the maintainer (Ntwali Bashige) an email at ntwali.bashige@gmail.com with a description of your contribution.
This step is important because before merging your contribution, I might have a discussion with you regarding your patch.

* Send a pull request.

### Contributing code

The original author (Ntwali Bashige) is not skilled with C++ therefore, contributing code is easy to improve performance or fix bugs.  
Please keep your patch small. Huge patches that try to fix too many things will be rejected. You can fix a lot of things if they are all similar.
For instance, at the moment, I am catching exceptions by value instead of by const reference. Submitting a patch that solves this is acceptable because there is no logic is there to understand.

### Contributing documentation

The documentation is always update and ripe for contribution to fix typography mistakes, grammatical errors and clarifying concepts.

### Contributing tests

Test, test and test. We can't have enough of those. Even one single test that demonstrates a specific corner case is always welcome.

## Code formatting guidelines

* Use 4 spaces (never tabs) to ident code.

* Use whitespace for readability.

* User underscore_case for C++ identifiers.

* Follow the Avalon documentation style guide for Avalon code to feature in the SDK.

* Stick to ASCII characters in the source and avoid Unicode characters.

## Git guidelines

* Do not work on the master branch. Create a branch with your Github name and work from the same.

* Use descriptive messages so that your commits make sense and match our email exchange.

## Conclusion

This document will continue to be updated as the language evolve and more people contribute.
I repeat again: be civil in all your exchanges with the community.
