#include "Movie.h"
#include <QRegularExpression>

Movie::Movie()
    : m_id(0)
    , m_rating(-1.0)
{
}

Movie::Movie(int id,
             const QString &movieId,
             const QString &title,
             const QString &year,
             const QString &genre,
             const QString &overview,
             const QString &director,
             const QString &cast,
             double rating,
             const QString &posterUrl)
    : m_id(id)
    , m_movieId(movieId.trimmed())
    , m_title(title.trimmed())
    , m_year(year.trimmed())
    , m_genre(genre.trimmed())
    , m_overview(overview.trimmed())
    , m_director(director.trimmed())
    , m_cast(cast.trimmed())
    , m_rating(rating)
    , m_posterUrl(posterUrl.trimmed())
{
    m_soup = generateSoup();
}

int Movie::getId() const { return m_id; }
QString Movie::getMovieId() const { return m_movieId; }
QString Movie::getTitle() const { return m_title; }
QString Movie::getYear() const { return m_year; }
QString Movie::getGenre() const { return m_genre; }
QString Movie::getOverview() const { return m_overview; }
QString Movie::getDirector() const { return m_director; }
QString Movie::getCast() const { return m_cast; }
double Movie::getRating() const { return m_rating; }
QString Movie::getPosterUrl() const { return m_posterUrl; }
QString Movie::getSoup() const { return m_soup.isEmpty() ? generateSoup() : m_soup; }

void Movie::setId(int id) { m_id = id; }
void Movie::setMovieId(const QString &movieId) { m_movieId = movieId.trimmed(); }
void Movie::setTitle(const QString &title) { m_title = title.trimmed(); }
void Movie::setYear(const QString &year) { m_year = year.trimmed(); }
void Movie::setGenre(const QString &genre) { m_genre = genre.trimmed(); }
void Movie::setOverview(const QString &overview) { m_overview = overview.trimmed(); }
void Movie::setDirector(const QString &director) { m_director = director.trimmed(); }
void Movie::setCast(const QString &cast) { m_cast = cast.trimmed(); }
void Movie::setRating(double rating) { m_rating = rating; }
void Movie::setPosterUrl(const QString &posterUrl) { m_posterUrl = posterUrl.trimmed(); }
void Movie::setSoup(const QString &soup) { m_soup = soup; }

QString Movie::generateSoup() const {
    // Exact Python logic: genre + " " + overview + " " + director + " " + cast
    QStringList parts;
    if (!m_genre.isEmpty()) parts << m_genre;
    if (!m_overview.isEmpty()) parts << m_overview;
    if (!m_director.isEmpty()) parts << m_director;
    if (!m_cast.isEmpty()) parts << m_cast;
    return parts.join(" ");
}

QString Movie::getFormattedYear() const {
    if (m_year.isEmpty()) return "N/A";
    
    // Extract 4 consecutive digits if present (e.g. from 'V2023' or '2023')
    static const QRegularExpression rx("(\\d{4})");
    auto match = rx.match(m_year);
    if (match.hasMatch()) {
        return match.captured(1);
    }
    return m_year;
}

QString Movie::getDisplayRating() const {
    if (m_rating < 0.0) {
        return "N/A";
    }
    return QString::number(m_rating, 'f', 1);
}

QStringList Movie::getGenreList() const {
    QStringList list;
    if (m_genre.isEmpty()) return list;
    const auto items = m_genre.split(',', Qt::SkipEmptyParts);
    for (const auto &item : items) {
        list << item.trimmed();
    }
    return list;
}

QStringList Movie::getCastList() const {
    QStringList list;
    if (m_cast.isEmpty()) return list;
    const auto items = m_cast.split(',', Qt::SkipEmptyParts);
    for (const auto &item : items) {
        list << item.trimmed();
    }
    return list;
}

bool Movie::isValid() const {
    return !m_title.isEmpty();
}
