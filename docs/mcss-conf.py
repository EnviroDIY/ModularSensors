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
            ("Library Dependencies", "page_library_dependencies",),
            ("Physical Dependencies", "page_physical_dependencies",),
            ("Terminology", "page_library_terminology",),
        ],
    ),
    (
        "FAQs",
        "page_faq",
        [
            ("Processor Compatibility", "page_processor_compatibility",),
            ("Arduino Streams and Software Serial", "page_arduino_streams",),
            ("Power Draw over Data Lines", "page_power_parasites",),
            ("Decreasing Memory Footprint", "page_memory_use"),
            ("In-Library Debugging", "page_code_debugging",),
            ("For Developers", "page_for_developers",),
        ],
    ),
    ("Modules", "modules", []),
    ("Classes", "annotated", [],),
    ("Source Files", "files", [],),
    (
        "Examples",
        "page_the_examples",
        [],
        # [
        #     (
        #         "Basic Functionality",
        #         "examples_basic",
        #         [
        #             ("Single Sensor", "example_single_sensor"),
        #             ("Simple Logging", "example_simple_logging"),
        #             ("Learn EnviroDIY", "example_learn_envirodiy"),
        #         ],
        #     ),
        #     (
        #         "Publishing Data",
        #         "examples_publishing",
        #         [
        #             ("Publishing to Monitor My Watershed", "example_mmw"),
        #             ("Publishing to ThingSpeak", "example_thingspeak"),
        #         ],
        #     ),
        #     (
        #         "Calculations and Complex Logging",
        #         "examples_complex",
        #         [
        #             ("Barometric Pressure Correction", "example_baro_rho"),
        #             ("Multiple Logging Intervals", "example_double_log"),
        #             ("Minimizing Cell Data Usage", "example_data_saving"),
        #         ],
        #     ),
        #     (
        #         "DRWI Citizen Science",
        #         "examples_drwi",
        #         [
        #             ("DRWI LTE", "example_drwi_lte"),
        #             ("DRWI CitSci (2G", "example_drwi_2g"),
        #             ("DRWI CitSci No Cellular", "example_drwi_no_cell"),
        #         ],
        #     ),
        #     (
        #         "Everything at Once",
        #         "examples_everything",
        #         [("The a la carte Menu", "example_menu"),],
        #     ),
        # ],
    ),
    ("More", "pages", [],),
]
LINKS_NAVBAR2 = []
VERSION_LABELS = True
CLASS_INDEX_EXPAND_LEVELS = 2

STYLESHEETS = [
    "css/m-EnviroDIY+documentation.compiled.css",
]
