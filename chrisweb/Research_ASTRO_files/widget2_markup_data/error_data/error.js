// Copyright 2010 Google Inc.  All Rights Reserved.

function clk(err, detail, cv) {
  if (document.images) {
    var u = '/maps/gen_204?source=ge_err_pg'
        + '&err=' + encodeURIComponent(err ? err : '')
        + '&cv=' + encodeURIComponent(cv ? cv : '')
        + '&detail=' + encodeURIComponent(detail ? detail : '');
    (new Image()).src = u;
  }
}

function clk1() {
  clk('CLICK_INSTALL_ONE_CLICK', '', '');
}

function clkS() {
  clk('CLICK_INSTALL_STUB', '', '');
}

function clkSC() {
  clk('CLICK_INSTALL_STUB_CHROME', '', '');
}

/**
 * Returns installation function call as a string.
 * @return {string} installation function.
 */
function getInstallLink() {
  var url = "";
  var userAgent = navigator.userAgent;
  if ((userAgent.indexOf('Chrome/1.') >=0) ||
      (userAgent.indexOf('Chrome/0.') >=0)) {
    url = 'clkSC();installViaStubForChrome();';
  } else {
    if (window.google.earth.isOneClickAvailable()) {
      var BROWSER_IE = 2;
      var BROWSER_FIREFOX = 3;
      if (BROWSER_FIREFOX == window.google.earth.getBrowserId()) {
        // Work around Firefox 3 issue by delaying install after setup.
        url = 'clk1();setTimeout(window.google.earth.installViaOneClick,1000);';
      } else {
        url = 'clk1();window.google.earth.installViaOneClick();';
      }
    } else {
      url = 'clkS();window.google.earth.installViaStub();';
    }
  }
  return url;
}

function setInstallLinks() {
  var installImmediately = 'javascript:' + getInstallLink() + 'void(0);';
  var alist = document.getElementsByName('install');
  for (var i = 0; i < alist.length; i++) {
    alist[i].href = installImmediately;
  }
}

function installViaStubForChrome() {
  document.location.hash = 'error=SUCCESS_RECENT_INSTALL_CHROME_RELOAD';
  window.location.reload(false);
}

function setSupportedBrowserText() {
  var userAgent = navigator.userAgent;
  var showOSText = '';

  if (userAgent.indexOf('Windows') >= 0) {
    showOSText = 'win';
  } else if (userAgent.indexOf('Macintosh') >= 0) {
    showOSText = 'mac';
  }

  if (showOSText) {
    var spanlist = document.getElementsByName(showOSText + '_supported_browsers');
    for (var i = 0; i < spanlist.length; i++) {
      spanlist[i].style.display = '';
    }
  }
}

function init() {
  window.google.earth.doOneClickSetup();
  setInstallLinks();
  setSupportedBrowserText();

  // Decode URL params.
  var params = {};
  var paramstring = document.location.hash.replace(/(^#)/,'');
  var pairs = paramstring.split('&');
  for (var i = 0; i < pairs.length; i++) {
    var p = pairs[i].split('=');
    params[decodeURIComponent(p[0])] = decodeURIComponent(p[1]);
  }

  var textElem = document.getElementById('text_default');

  // If 'error' is defined, try to show the corresponding info section.
  var error = params['error'];
  var autoDownload = false;
  if (error) {
    // We alias some groups of errors to the same message.
    var alias = error;
    var aliases = {
      'ERR_INVALID_DIV': 'ERR_CREATE_PLUGIN',
      'ERR_NO_AVAILABLE_ID': 'ERR_CREATE_PLUGIN',
      'ERR_EARTH_CONNECT_TIMEOUT': 'ERR_BRIDGE',
      'ERR_CREATE_CONNECT_MUTEX': 'ERR_CREATE_EARTH'
    };
    if (aliases[error]) {
      alias = aliases[error];
    }

    if (error == 'INSTALLING') {
      // Show 'Installing' page while popping up download.
      autoDownload = true;
    } else if (alias == 'VIEW_INSTALLING') {
      // Show "Installing' page without popping up download.
      alias = 'INSTALLING';
    }

    if (alias == 'INSTALLING') {
      // Adwords conversion logging.
      var conversionIframe = document.createElement('iframe');
      conversionIframe.style.display = 'none';
      conversionIframe.src = 'http://earth.google.com/plugin/conversion.html';
      document.body.appendChild(conversionIframe);
    }

    var elem = document.getElementById('text_' + alias);
    if (elem) {
      textElem = elem;
    }
  }

  // Show default error info.
  textElem.style.display = '';

  // Enable installer div if necessary
  if (textElem.className.indexOf('show_installer') >= 0) {
    document.getElementById('installer').style.display = '';
  }

  if (autoDownload) {
    var installLink = getInstallLink();
    eval(installLink);
  }

  // Log.
  var detail = params['detail'];
  detail = detail ? detail : '';
  var cv = '';
  var pieces = detail.split(',');
  detail = pieces[0];
  if (pieces[1]) {
    var vpieces = pieces[1].split(':');
    if (vpieces[0] == 'pluginversion') {
      cv = vpieces[1];
    }
  }
  clk(error, detail, cv);

  function doCheckInstalledLoop() {
    navigator.plugins.refresh(false);
    if (google.earth.isInstalled()) {
      var userAgent = navigator.userAgent;
      if (userAgent.indexOf('Windows') >= 0) {
        if (userAgent.indexOf('MSIE') >= 0) {
          document.location.hash = 'error=SUCCESS_RECENT_INSTALL_RESTART';
        } else {
          document.location.hash = 'error=SUCCESS_RECENT_INSTALL_RELOAD';
        }
      } else if (userAgent.indexOf('Macintosh') >= 0) {
        if (userAgent.indexOf('Firefox') >= 0) {
          document.location.hash = 'error=SUCCESS_RECENT_INSTALL_RELOAD';
        } else {
          document.location.hash = 'error=SUCCESS_RECENT_INSTALL_RESTART';
        }
      }
      window.location.reload(false);
    } else {
      setTimeout(doCheckInstalledLoop, 2000);
    }
  }

  if (error == 'SUCCESS_RECENT_INSTALL_CHROME_RELOAD') {
    setTimeout(window.google.earth.installViaStub,1000);
  } else {
    try {
      if (google.earth.isSupported()) {
        // In Win/Chrome, the plugin installer triggers the ERR_NOT_INSTALLED
        // page to reload, in which case we still want to run the "check
        // install loop" regardless of whether the plugin is already installed
        // so it shows the success page as soon as the installation completes.
        var winChromeErrNotInstalled =
            navigator.userAgent.indexOf('Windows') >= 0 &&
            navigator.userAgent.indexOf('Chrome') >= 0 &&
            alias == 'ERR_NOT_INSTALLED';
        if (!google.earth.isInstalled() ||
            winChromeErrNotInstalled) {
          doCheckInstalledLoop();
        }
      }
    } catch (err) {
    }
  }
}
