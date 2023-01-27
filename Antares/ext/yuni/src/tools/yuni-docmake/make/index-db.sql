
--
-- Header
--
CREATE TABLE index_header (
	-- Version of the index cache
	version                INTEGER NOT NULL DEFAULT 1,
	-- Flag to know if the whole index cache is marked as dirty
	-- (means potentially invalid)
	dirty                  INTEGER NOT NULL DEFAULT 0
);



--
-- Articles
--
CREATE TABLE articles (
	id                     INTEGER PRIMARY KEY AUTOINCREMENT,
	-- Lang of the artcile
	lang                   TEXT NOT NULL DEFAULT 'en',
	-- original filename (absolute)
	rel_path               TEXT NOT NULL,
	-- Target filename, relative to the htdocs
	html_href              TEXT NOT NULL UNIQUE,
	-- Article's title
	title                  TEXT NOT NULL,
	-- SEO weight
	weight                 REAL NOT NULL DEFAULT 1.0,
	-- Order
	parent_order           INTEGER NOT NULL DEFAULT 1000,
	-- Flag to know if the article should be rebuilt
	dirty                  INTEGER NOT NULL DEFAULT 1,
	-- The number of tags
	tag_count              INTEGER NOT NULL DEFAULT 0,
	-- Flag to show the quick links
	show_quick_links       INTEGER NOT NULL DEFAULT 1,
	-- Flag to show the history
	show_history           INTEGER NOT NULL DEFAULT 1,
	-- Flag to show the table of content
	show_toc               INTEGER NOT NULL DEFAULT 1,
	-- Timestamp of the last modification
	modified               INTEGER NOT NULL DEFAULT 0,
	-- Flag to show the directory index
	directory_index        INTEGER NOT NULL DEFAULT 2, -- dicAll
	-- The parent
	parent                 TEXT NOT NULL,
	force_access_path      TEXT DEFAULT NULL,
	dir_index              TEXT DEFAULT NULL
);

CREATE INDEX ix_art_relpath ON articles(rel_path);
CREATE INDEX ix_art_parent ON articles(parent);
CREATE INDEX ix_art_dirty ON articles(dirty);



--
-- Table of content
--
CREATE TABLE toc (
	html_href              TEXT NOT NULL REFERENCES articles(html_href) ON DELETE CASCADE,
	indx                   INTEGER NOT NULL,
	lvl                    INTEGER NOT NULL,
	href_id                TEXT NOT NULL,
	caption                TEXT NOT NULL,
	PRIMARY KEY (html_href,indx)
);


--
-- SEO, terms
--
CREATE TABLE terms (
	id                     INTEGER PRIMARY KEY AUTOINCREMENT,
	weight_user            REAL NOT NULL DEFAULT 1.0,
	weight_rel_others      REAL NOT NULL DEFAULT 1.0,
	-- The total weight
	weight                 REAL NOT NULL DEFAULT 1.0,
	term                   TEXT NOT NULL UNIQUE
);


--
-- SEO
--
CREATE TABLE terms_per_article (
	term_id                INTEGER NOT NULL REFERENCES terms(id) ON DELETE CASCADE,
	article_id             INTEGER NOT NULL REFERENCES articles(id) ON DELETE CASCADE,
	count_in_page          INTEGER NOT NULL DEFAULT 0,
	weight                 REAL NOT NULL DEFAULT 1.0
);


--
-- Tags per article
--
CREATE TABLE tags_per_article (
	article_id             INTEGER NOT NULL REFERENCES articles(id) ON DELETE CASCADE,
	tagname                TEXT NOT NULL,
	PRIMARY KEY (article_id, tagname)
);


--
-- Dependencies
--
CREATE TABLE deps (
	input                  TEXT NOT NULL PRIMARY KEY,
	html_href              TEXT NOT NULL,
	article_id             INTEGER NOT NULL REFERENCES articles(id) ON DELETE CASCADE
);

