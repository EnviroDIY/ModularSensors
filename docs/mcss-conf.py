DOXYFILE = 'mcss-Doxyfile'
THEME_COLOR = '#cb4b16'
FAVICON = 'https://3qzcxr28gq9vutx8scdn91zq-wpengine.netdna-ssl.com/wp-content/uploads/2016/05/cropped-EnviroDIY_LogoMaster_TrueSquare_V5_TwoTree_Trans_notext-192x192.png'
LINKS_NAVBAR1 = [
    ("Pages", 'pages', [("Pages", 'pages')]),
    ("Modules", 'modules', [])
]
LINKS_NAVBAR2 = [
    ("Classes", 'annotated', []),
    ("Files", 'files', [])
]
VERSION_LABELS = True
CLASS_INDEX_EXPAND_LEVELS = 2

STYLESHEETS = [
    'https://fonts.googleapis.com/css?family=Roboto+Condensed%3A300%2C300i%2C400%2C400i%2C700%2C700i%7CRoboto%3A300%2C300i%2C400%2C400i%2C500%2C500i%2C700%2C700i&display=swap&ver=4.2.1',
    'https://fonts.googleapis.com/css?family=Open+Sans%3A400%2C400i%2C700%2C700i&display=swap&ver=4.2.1',
    'https://fonts.googleapis.com/css?family=Source+Code+Pro&display=swap&ver=4.2.1',
    'https://fonts.googleapis.com/css?family=Libre+Baskerville:400,400i,700,700i%7CSource+Code+Pro:400,400i,600',
    'https://3qzcxr28gq9vutx8scdn91zq-wpengine.netdna-ssl.com/wp-content/themes/socialize/style.css?ver=5.4.2',
    'https://3qzcxr28gq9vutx8scdn91zq-wpengine.netdna-ssl.com/wp-content/themes/socialize-child/style.css?ver=5.4.2',
    'css/m-EnviroDIY+documentation.compiled.css'
]
FINE_PRINT = '''
  <div class="m-container">
    <div class="m-row">
      <div class="m-col-l-10 m-push-l-1">
        <p>ModularSensors An Arduino library to give environmental sensors a common interface of functions for use with Arduino-framework dataloggers, such as the EnviroDIY Mayfly.</p>
        <p>Documentation created with <a href="https://doxygen.org/">Doxygen</a> {doxygen_version} and <a href="https://mcss.mosra.cz/">m.css</a>.</p>
      </div>
    </div>
  </div>
'''