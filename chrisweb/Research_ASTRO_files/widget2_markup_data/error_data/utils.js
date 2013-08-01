// Copyright 2008 Google Inc.  All Rights Reserved.

/**
 * @fileoverview google earth plugin installation related
 * utilities.
 */

if (!('google' in window)) {
  window.google = {};
}
if (!('earth' in window.google)) {
  window.google.earth = {};
}

/**
 * Setup google one click
 */
window.google.earth.doOneClickSetup = function() {
  if ('win' == _GU_getPlatform()) {
    _GU_SetupOneClick();
  }
}

/**
 * Check if one click is available
 *
 * @return True if one click is available else False.
 */
window.google.earth.isOneClickAvailable = function() {
  return _GU_isOneClickAvailable();
}

/**
 * Get broswer id from one click.
 *
 * @return
 *   0 for Unknown browser
 *   1 for default browser
 *   2 for Internet Explorer
 *   3 for Firefox
 */
window.google.earth.getBrowserId = function() {
  return _GU_getBrowserId();
}

/**
 * Build an application object for google earth plug-in
 * @param {boolean} isBeta Whether or not the download is the beta version.
 * @return {Object} object that contain all the earth plug-in
 *         info.
 */
window.google.earth.buildAppsInfo = function(isBeta) {
  return [_GU_createAppInfo('{2BF2CA35-CCAF-4E58-BAB7-4163BFA03B88}',
                            'Google Earth Plug-in',
                            'True',
                            (isBeta ? '&ap=dev' : '') + '&BRAND=GGEP')
         ];
}

/**
 * Get preferred language, from the Google pref cookie if it is set,
 * or this url path if the cookie is not set or is not valid.
 */
window.google.earth.getPrefLang = function() {
  function readCookie(name) {
    var nameEQ = name + "=";
    var ca = document.cookie.split(';');
    for (var i = 0; i < ca.length; i++) {
      var c = ca[i];
      while (c.charAt(0)==' ') {
        c = c.substring(1,c.length);
      }
      if (c.indexOf(nameEQ) == 0) {
        return c.substring(nameEQ.length,c.length);
      }
    }
    return null;
  }

  // Return true if the specified language is supported by Omaha.
  var omahaSupportsLanguage = function(lang) {
    var valid_languages = [ 'ar', 'bg', 'ca', 'cs', 'da', 'de', 'el', 'en',
                            'en-GB', 'es', 'fi', 'fil', 'fr', 'hi', 'hr',
                            'hu', 'id', 'it', 'iw', 'ja', 'ko', 'lt', 'lv',
                            'nl', 'no', 'pl', 'pt-BR', 'pt-PT', 'ro', 'ru',
                            'sk', 'sl', 'sr', 'sv', 'th', 'tr', 'uk', 'vi',
                            'zh-CN', 'zh-TW' ];
    var index = -1;
    for (var i = 0; i < valid_languages.length; i++) {
      if (valid_languages[i] == lang) {
        index = i;
        break;
      }
    }
    return index != -1;
  };

  var foundLanguage = false;
  var lang = 'en';

  // Try pref cookie.
  var prefCookie = readCookie('PREF');
  if (prefCookie) {
    var keys = prefCookie.split(':');
    for (var i = 0; i < keys.length; i++) {
      // check for the language key
      if (keys[i].indexOf('LD=') == 0) {
        var hash = keys[i].split('=');
        if (hash.length >= 2) {
          lang = hash[1];
          foundLanguage = true;
        }
      }
    }

    if (foundLanguage && !omahaSupportsLanguage(lang)) {
      foundLanguage = false;
    }
  }

  if (!foundLanguage) {
    // Try url path.
    var path = document.location.pathname;
    if (path.indexOf('/intl/') == 0) {
      var beginIndex = 6;
      var endIndex = path.indexOf('/', beginIndex);
      if (endIndex != -1) {
        lang = path.slice(beginIndex, endIndex);
        if (omahaSupportsLanguage(lang)) {
          foundLanguage = true;
        }
      }
    }
  }

  if (!foundLanguage) {
    lang = 'en';
  }
  return lang;
}

/**
 * Install earth plugin using stub installer.
 * @param {boolean} isBeta Whether or not the download is the beta version.
 */
window.google.earth.installViaStub = function(isBeta) {
  if ('win' == _GU_getPlatform()) {
    // Build a download url for the stub installer.
    var exeName = isBeta ? 'googleearthplugindevsetup.exe' :
                              'GoogleEarthPluginSetup.exe';
    top.location = _GU_buildDlPath(window.google.earth.buildAppsInfo(isBeta),
                                   window.google.earth.getPrefLang(),
                                   false,
                                   'http://dl.google.com',
                                   '/earth/plugin/' + exeName);
  }

  if ('mac' == _GU_getPlatform()) {
    var userAgent = navigator.userAgent;
    if (userAgent.indexOf('Intel Mac') >=0) {
      if (isBeta) {
        top.location = 'http://cache.pack.google.com/edgedl/earth/plugin/current/googleearth-mac-plugin-dev-intel.dmg';
      } else {
        top.location = 'http://cache.pack.google.com/edgedl/earth/plugin/current/googleearth-mac-plugin-intel.dmg';
      }
    } else {
      if (isBeta) {
        top.location = 'http://cache.pack.google.com/edgedl/earth/plugin/current/googleearth-mac-plugin-dev-universal.dmg';
      } else {
        top.location = 'http://cache.pack.google.com/edgedl/earth/plugin/current/googleearth-mac-plugin-universal.dmg';
      }
    }
  }
}

/**
 * Install earth plugin using the one click.
 */
window.google.earth.installViaOneClick = function() {

  // Called after the <nop>OneClick install has started.
  // The app has not yet been installed.
  function installSuccess_() {
  }

  // Called if the OneClick install failed to start.
  // This is not called if the app fails to install.
  function installFailure_() {
    window.google.earth.installViaStub();
  }

  window.google.update.oneclick.install(window.google.earth.buildAppsInfo(),
                                        window.google.earth.getPrefLang(),
                                        false,
                                        function() { installSuccess_(); },
                                        function() { installFailure_(); });
}

/**
 * Get all required install flags.
 * @return {string} string that represents all the install
 *         flags.
 */
window.google.earth.getInstallFlags = function() {

  var extraFlags = '';

  // turn off browser restart for all
  var browser = '&browser=1';
  extraFlags += browser;

  return extraFlags;
}
