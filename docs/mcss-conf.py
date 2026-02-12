DOXYFILE = "mcss-Doxyfile"
THEME_COLOR = "#cb4b16"
# FAVICON = "https://3qzcxr28gq9vutx8scdn91zq-wpengine.netdna-ssl.com/wp-content/uploads/2016/05/cropped-EnviroDIY_LogoMaster_TrueSquare_V5_TwoTree_Trans_notext-192x192.png"
FAVICON = "MS_FavIcon.png"
LINKS_NAVBAR1 = [
    (
        "About",
        "index",
        [
            ('<a href="index.html#data_receivers">Data Endpoints</a>',),
            ('<a href="index.html#supported_sensors">Supported Sensors</a>',),
            ('<a href="index.html#modems">Supported Cellular/Wifi Modules</a>',),
            ('<a href="index.html#contributing">Contributing</a>',),
            ('<a href="index.html#license">License</a>',),
            ('<a href="index.html#acknowledgments">Acknowledgments</a>',),
            ('<a href="change_log.html">ChangeLog</a>',),
        ],
    ),
    (
        "Getting Started",
        "page_getting_started",
        [
            (
                "Getting Started",
                "page_getting_started",
                # [
                #     (
                #         '<a href="page_getting_started.html#getting_started_ide">IDE and Driver Installation</a>',
                #     ),
                #     (
                #         '<a href="page_getting_started.html#getting_started_libraries">Library Installation</a>',
                #     ),
                #     ('<a href="page_getting_started.html#getting_started_clock">Setting the Clock</a>',),
                #     (
                #         '<a href="page_getting_started.html#getting_started_program">Writing Your Logger Program</a>',
                #     ),
                #     (
                #         '<a href="page_getting_started.html#getting_started_examples">Modifying the Examples</a>',
                #     ),
                #     (
                #         '<a href="page_getting_started.html#getting_started_deploying">Deploying your Station</a>',
                #     ),
                #     (
                #         '<a href="page_getting_started.html#page_getting_started_other">Other Helpful Pages</a>',
                #     ),
                # ],
            ),
            (
                "Library Dependencies",
                "page_library_dependencies",
            ),
            (
                "Physical Dependencies",
                "page_physical_dependencies",
            ),
            (
                "Terminology",
                "page_library_terminology",
            ),
            (
                "Other Sensor and Modem Notes",
                "page_other_notes",
            ),
        ],
    ),
    # (
    #     "FAQs",
    #     "page_faq",
    #     [
    #         (
    #             "Processor Compatibility",
    #             "page_processor_compatibility",
    #         ),
    #         (
    #             "Arduino Streams and Software Serial",
    #             "page_arduino_streams",
    #         ),
    #         (
    #             "Power Draw over Data Lines",
    #             "page_power_parasites",
    #         ),
    #         ("Decreasing Memory Footprint", "page_memory_use"),
    #         (
    #             "In-Library Debugging",
    #             "page_code_debugging",
    #         ),
    #     ],
    # ),
    ("Modules", "topics", []),
    (
        "Classes",
        "annotated",
        [],
    ),
    (
        "Source Files",
        "files",
        [],
    ),
    (
        "Examples",
        "page_the_examples",
        [],
    ),
    (
        "More",
        "pages",
        [],
    ),
]
LINKS_NAVBAR2 = []
VERSION_LABELS = True
CLASS_INDEX_EXPAND_LEVELS = 2

STYLESHEETS = [
    "css/m-EnviroDIY+documentation.compiled.css",
]

EXTRA_FILES = [
    "gp-desktop-logo.png",
    "gp-mobile-logo.png",
    "gp-scrolling-logo.png",
    "clipboard.js",
]
DESKTOP_LOGO = "gp-desktop-logo.png"
MOBILE_LOGO = "gp-mobile-logo.png"
SCROLLING_LOGO = "gp-scrolling-logo.png"
M_MATH_RENDER_AS_CODE = False
M_MATH_CACHE_FILE = "m.math.cache"
