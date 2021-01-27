﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2021 Ryo Suzuki
//	Copyright (c) 2016-2021 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Polygon.hpp>
# include <Siv3D/Shape2D.hpp>
# include <Siv3D/LineString.hpp>
# include <Siv3D/Math.hpp>
# include <Siv3D/HashSet.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/Cursor.hpp>
# include "PolygonDetail.hpp"
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4100)
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4127)
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4244)
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4267)
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4819)
# include <boost/geometry/algorithms/is_valid.hpp>
# include <boost/geometry/algorithms/correct.hpp>
# include <ThirdParty/boost/geometry/extensions/algorithms/dissolve.hpp>
SIV3D_DISABLE_MSVC_WARNINGS_POP()
SIV3D_DISABLE_MSVC_WARNINGS_POP()
SIV3D_DISABLE_MSVC_WARNINGS_POP()
SIV3D_DISABLE_MSVC_WARNINGS_POP()
SIV3D_DISABLE_MSVC_WARNINGS_POP()
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Common/Siv3DEngine.hpp>

namespace s3d
{
	namespace detail
	{
		[[nodiscard]]
		static bool HasSamePoints(const Vec2* pVertex, const size_t vertexSize)
		{
			return (HashSet<Vec2>(pVertex, pVertex + vertexSize).size() != vertexSize);
		}

		[[nodiscard]]
		constexpr PolygonFailureType Convert(const boost::geometry::validity_failure_type failure) noexcept
		{
			// https://www.boost.org/doc/libs/1_72_0/libs/geometry/doc/html/geometry/reference/enumerations/validity_failure_type.html
			switch (failure)
			{
			case boost::geometry::no_failure:
				return PolygonFailureType::OK;
			case boost::geometry::failure_few_points:
				return PolygonFailureType::FewPoints;
			case boost::geometry::failure_wrong_topological_dimension:
				return PolygonFailureType::WrongTopologicalDimension;
			case boost::geometry::failure_spikes:
				return PolygonFailureType::Spikes;
			case boost::geometry::failure_duplicate_points:
				return PolygonFailureType::DuplicatePoints;
			case boost::geometry::failure_not_closed:
				return PolygonFailureType::NotClosed;
			case boost::geometry::failure_self_intersections:
				return PolygonFailureType::SelfIntersections;
			case boost::geometry::failure_wrong_orientation:
				return PolygonFailureType::WrongOrientation;
			case boost::geometry::failure_interior_rings_outside:
				return PolygonFailureType::InteriorRingsOutside;
			case boost::geometry::failure_nested_interior_rings:
				return PolygonFailureType::NestedInteriorRings;
			case boost::geometry::failure_disconnected_interior:
				return PolygonFailureType::DisconnectedInterior;
			case boost::geometry::failure_intersecting_interiors:
				return PolygonFailureType::IntersectingInteriors;
			case boost::geometry::failure_wrong_corner_order:
				return PolygonFailureType::WrongCornerOrder;
			case boost::geometry::failure_invalid_coordinate:
				return PolygonFailureType::InvalidCoordinate;
			default:
				return PolygonFailureType::Unknown;
			}
		}
	}

	Polygon::Polygon()
		: pImpl{ std::make_unique<PolygonDetail>() }
	{

	}

	Polygon::Polygon(const Polygon& polygon)
		: pImpl{ std::make_unique<PolygonDetail>(*polygon.pImpl) }
	{

	}

	Polygon::Polygon(Polygon&& polygon) noexcept
		: pImpl{ std::move(polygon.pImpl) }
	{

	}

	Polygon::Polygon(const Vec2* outer, const size_t size, Array<Array<Vec2>> holes, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, size, std::move(holes), skipValidation) }
	{

	}

	Polygon::Polygon(const Array<Vec2>& outer, Array<Array<Vec2>> holes, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer.data(), outer.size(), std::move(holes), skipValidation) }
	{

	}

	Polygon::Polygon(const Array<Vec2>& outer, const Array<TriangleIndex>& indices, const RectF& boundingRect, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer.data(), outer.size(), indices, boundingRect, skipValidation) }
	{

	}

	Polygon::Polygon(const Array<Vec2>& outer, Array<Array<Vec2>> holes, const Array<Float2>& vertices, const Array<TriangleIndex>& indices, const RectF& boundingRect, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, std::move(holes), vertices, indices, boundingRect, skipValidation) }
	{

	}

	Polygon::Polygon(std::initializer_list<Vec2> outer, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer.begin(), outer.size(), Array<Array<Vec2>>{}, skipValidation) }
	{

	}

	Polygon::Polygon(const Shape2D& shape)
		: pImpl(std::make_unique<PolygonDetail>(shape.vertices().data(), shape.vertices().size(), shape.indices()))
	{

	}

	Polygon::~Polygon()
	{

	}

	Polygon& Polygon::operator =(const Polygon& polygon)
	{
		*pImpl = *polygon.pImpl;
		
		return *this;
	}

	Polygon& Polygon::operator =(Polygon&& polygon) noexcept
	{
		pImpl = std::move(polygon.pImpl);

		return *this;
	}

	bool Polygon::isEmpty() const noexcept
	{
		return pImpl->outer().isEmpty();
	}

	bool Polygon::hasHoles() const noexcept
	{
		return (not pImpl->inners().isEmpty());
	}

	size_t Polygon::num_holes() const noexcept
	{
		return pImpl->inners().size();
	}

	const Array<Vec2>& Polygon::outer() const noexcept
	{
		return pImpl->outer();
	}

	const Array<Array<Vec2>>& Polygon::inners() const noexcept
	{
		return pImpl->inners();
	}

	const Array<Float2>& Polygon::vertices() const noexcept
	{
		return pImpl->vertices();
	}

	const Array<TriangleIndex>& Polygon::indices() const noexcept
	{
		return pImpl->indices();
	}

	const RectF& Polygon::boundingRect() const noexcept
	{
		return pImpl->boundingRect();
	}

	size_t Polygon::num_triangles() const noexcept
	{
		return pImpl->indices().size();
	}

	Triangle Polygon::triangle(const size_t index) const
	{
		const auto& indices = pImpl->indices();

		if (index >= indices.size())
		{
			throw std::out_of_range("Polygon::triangle(): index out of range");
		}

		const auto& vertices = pImpl->vertices();
		const auto& triangleIndex = indices[index];
	
		return{ vertices[triangleIndex.i0], vertices[triangleIndex.i1], vertices[triangleIndex.i2] };
	}

	Polygon& Polygon::addHole(Array<Vec2> hole, const SkipValidation skipValidation)
	{
		if (hole.size() < 3)
		{
			return *this;
		}

		Array<Array<Vec2>> inners(Arg::reserve = (pImpl->inners().size() + 1));
		{
			inners.append(pImpl->inners());
			inners.push_back(std::move(hole));
		}

		return (*this = Polygon{ pImpl->outer(), std::move(inners), skipValidation });
	}

	Polygon& Polygon::addHoles(Array<Array<Vec2>> holes, const SkipValidation skipValidation)
	{
		holes.remove_if([](const Array<Vec2>& hole) { return (hole.size() < 3); });

		if (not holes)
		{
			return *this;
		}

		Array<Array<Vec2>> inners(Arg::reserve = (pImpl->inners().size() + holes.size()));
		{
			inners.append(pImpl->inners());
			inners.append(holes);
		}

		return (*this = Polygon(pImpl->outer(), std::move(inners), skipValidation));
	}

	Polygon Polygon::movedBy(const Vec2 v) const
	{
		Polygon result{ *this };

		result.moveBy(v);

		return result;
	}

	Polygon& Polygon::moveBy(const Vec2 v) noexcept
	{
		pImpl->moveBy(v);

		return *this;
	}

	Polygon Polygon::rotatedAt(const Vec2 pos, const double angle) const
	{
		Polygon result{ *this };

		result.rotateAt(pos, angle);

		return result;
	}

	Polygon& Polygon::rotateAt(const Vec2 pos, const double angle)
	{
		pImpl->rotateAt(pos, angle);

		return *this;
	}

	Polygon Polygon::transformed(const double s, const double c, const Vec2& pos) const
	{
		Polygon result{ *this };

		result.transform(s, c, pos);

		return result;
	}

	Polygon& Polygon::transform(const double s, const double c, const Vec2& pos)
	{
		pImpl->transform(s, c, pos);

		return *this;
	}

	Polygon Polygon::scaled(const double s) const
	{
		Polygon result{ *this };

		result.scale(s);

		return result;
	}

	Polygon& Polygon::scale(const double s)
	{
		pImpl->scale(s);

		return *this;
	}

	Polygon Polygon::scaled(const Vec2 s) const
	{
		Polygon result{ *this };

		result.scale(s);

		return result;
	}

	Polygon& Polygon::scale(const Vec2 s)
	{
		pImpl->scale(s);

		return *this;
	}

	double Polygon::area() const noexcept
	{
		return pImpl->area();
	}

	double Polygon::perimeter() const noexcept
	{
		return pImpl->perimeter();
	}

	Vec2 Polygon::centroid() const
	{
		return pImpl->centroid();
	}

	Polygon Polygon::calculateConvexHull() const
	{
		return pImpl->calculateConvexHull();
	}

	Polygon Polygon::calculateBuffer(const double distance) const
	{
		if (distance == 0.0)
		{
			return *this;
		}

		return pImpl->calculateBuffer(distance);
	}

	Polygon Polygon::calculateRoundBuffer(const double distance) const
	{
		if (distance == 0.0)
		{
			return *this;
		}

		return pImpl->calculateRoundBuffer(distance);
	}

	Polygon Polygon::simplified(const double maxDistance) const
	{
		if (maxDistance <= 0.0)
		{
			return *this;
		}

		return pImpl->simplified(maxDistance);
	}

	LineString Polygon::outline(const CloseRing closeRing) const
	{
		const auto& out = outer();

		if (out.isEmpty())
		{
			return{};
		}

		if (closeRing)
		{
			LineString points;
			points.reserve(out.size() + 1);
			points.append(out);
			points.push_back(out.front());
			return points;
		}
		else
		{
			return LineString{ out };
		}
	}

	LineString Polygon::outline(double distanceFromOrigin, double length) const
	{
		if (length <= 0.0)
		{
			distanceFromOrigin += length;
			length = -length;
		}

		const auto& out = outer();
		const size_t N = out.size();
		Array<double> lens(N);
		{
			for (size_t i = 0; i < (N - 1); ++i)
			{
				lens[i] = out[i].distanceFrom(out[i + 1]);
			}

			lens[N - 1] = out[N - 1].distanceFrom(out[0]);
		}
		const double perim = lens.sum();

		distanceFromOrigin = Math::Fmod(distanceFromOrigin, perim) + (distanceFromOrigin < 0 ? perim : 0);
		length = Min(length, perim);
		const double distanceToTarget = (distanceFromOrigin + length);

		LineString points;
		double currentLength = 0.0;

		for (size_t n = 0; n < (N * 2); ++n)
		{
			const size_t i = (n % N);
			const double len = lens[i];
			const Vec2 pFrom = out[i];
			const Vec2 pTo = out[(N <= (i + 1)) ? (i - (N - 1)) : (i + 1)];

			if (not points)
			{
				if ((distanceFromOrigin <= (currentLength + len)))
				{
					const Vec2 origin = pFrom + (pTo - pFrom)
						.setLength(distanceFromOrigin - currentLength);
					points << origin;

					if (distanceToTarget <= (currentLength + len))
					{
						const Vec2 target = pFrom + (pTo - pFrom)
							.setLength(distanceToTarget - currentLength);
						points << target;
						break;
					}

					points << pTo;
				}
			}
			else
			{
				if (distanceToTarget <= (currentLength + len))
				{
					const Vec2 target = pFrom + (pTo - pFrom)
						.setLength(distanceToTarget - currentLength);
					points << target;
					break;
				}

				points << pTo;
			}

			currentLength += len;
		}

		return points;
	}

	bool Polygon::append(const RectF& other)
	{
		return pImpl->append(other);
	}

	bool Polygon::append(const Polygon& other)
	{
		return pImpl->append(other);
	}

	bool Polygon::intersects(const Line& other) const
	{
		return pImpl->intersects(other);
	}

	bool Polygon::intersects(const Rect& other) const
	{
		return pImpl->intersects(other);
	}

	bool Polygon::intersects(const RectF& other) const
	{
		return pImpl->intersects(other);
	}

	bool Polygon::intersects(const Polygon& other) const
	{
		return pImpl->intersects(*other.pImpl);
	}

	bool Polygon::leftClicked() const noexcept
	{
		return (MouseL.down() && mouseOver());
	}

	bool Polygon::leftPressed() const noexcept
	{
		return (MouseL.pressed() && mouseOver());
	}

	bool Polygon::leftReleased() const noexcept
	{
		return (MouseL.up() && mouseOver());
	}

	bool Polygon::rightClicked() const noexcept
	{
		return (MouseR.down() && mouseOver());
	}

	bool Polygon::rightPressed() const noexcept
	{
		return (MouseR.pressed() && mouseOver());
	}

	bool Polygon::rightReleased() const noexcept
	{
		return (MouseR.up() && mouseOver());
	}

	bool Polygon::mouseOver() const noexcept
	{
		return Geometry2D::Intersect(Cursor::PosF(), *this);
	}

	const Polygon& Polygon::draw(const ColorF& color) const
	{
		pImpl->draw(color);

		return *this;
	}

	void Polygon::draw(const Vec2& pos, const ColorF& color) const
	{
		pImpl->draw(pos, color);
	}

	const Polygon& Polygon::drawFrame(const double thickness, const ColorF& color) const
	{
		pImpl->drawFrame(thickness, color);

		return *this;
	}

	void Polygon::drawFrame(const Vec2& pos, const double thickness, const ColorF& color) const
	{
		pImpl->drawFrame(pos, thickness, color);
	}

	const Polygon& Polygon::drawWireframe(const double thickness, const ColorF& color) const
	{
		if (isEmpty())
		{
			return *this;
		}

		const auto& indices = pImpl->indices();
		const Float2* pVertex = pImpl->vertices().data();
		const TriangleIndex* pIndex = indices.data();
		const TriangleIndex* const pIndexEnd = (pIndex + indices.size());
		const Float4 colorF = color.toFloat4();

		while (pIndex != pIndexEnd)
		{
			const Float2 points[3] = { pVertex[pIndex->i0], pVertex[pIndex->i1], pVertex[pIndex->i2] };

			SIV3D_ENGINE(Renderer2D)->addPolygonFrame(points, 3, static_cast<float>(thickness), colorF);

			++pIndex;
		}

		return *this;
	}

	void Polygon::drawWireframe(const Vec2& pos, const double thickness, const ColorF& color) const
	{
		if (isEmpty())
		{
			return;
		}

		const auto& indices = pImpl->indices();
		const Float2* pVertex = pImpl->vertices().data();
		const TriangleIndex* pIndex = indices.data();
		const TriangleIndex* const pIndexEnd = (pIndex + indices.size());
		const Float4 colorF = color.toFloat4();
		const Float2 offset = pos;

		while (pIndex != pIndexEnd)
		{
			const Float2 points[3] = {
				(pVertex[pIndex->i0] + offset),
				(pVertex[pIndex->i1] + offset),
				(pVertex[pIndex->i2] + offset)
			};

			SIV3D_ENGINE(Renderer2D)->addPolygonFrame(points, 3, static_cast<float>(thickness), colorF);

			++pIndex;
		}
	}

	PolygonFailureType Polygon::Validate(const Vec2* pVertex, const size_t vertexSize, const Array<Array<Vec2>>& holes)
	{
		CwOpenPolygon polygon;
		auto& outer = polygon.outer();
		{
			outer.assign(pVertex, pVertex + vertexSize);

			polygon.inners().reserve(holes.size());

			for (const auto& hole : holes)
			{
				polygon.inners().emplace_back(hole.begin(), hole.end());
			}
		}

		boost::geometry::validity_failure_type failure;
		bool valid = boost::geometry::is_valid(polygon, failure);

		if (valid)
		{
			// 頂点の重複は boost::geometry::is_valid() で取得できないので、
			// HashSet を使って計算
			if (detail::HasSamePoints(outer.data(), outer.size()))
			{
				valid = false;
				failure = boost::geometry::failure_duplicate_points;
			}
		}

		if (valid)
		{
			for (const auto& inner : polygon.inners())
			{
				if (detail::HasSamePoints(inner.data(), inner.size()))
				{
					valid = false;
					failure = boost::geometry::failure_duplicate_points;
				}
			}
		}

		return detail::Convert(failure);
	}

	Array<Polygon> Polygon::Correct(const Vec2* pVertex, const size_t vertexSize, const Array<Array<Vec2>>& holes)
	{
		CwOpenPolygon polygon;
		polygon.outer().assign(pVertex, pVertex + vertexSize);

		for (const auto& hole : holes)
		{
			polygon.inners().emplace_back(hole.begin(), hole.end());
		}

		{
			boost::geometry::validity_failure_type failure;
			bool valid = boost::geometry::is_valid(polygon, failure);

			if (valid)
			{
				// 頂点の重複は boost::geometry::is_valid() で取得できないので、
				// HashSet を使って計算
				if (detail::HasSamePoints(polygon.outer().data(), polygon.outer().size()))
				{
					valid = false;
					failure = boost::geometry::failure_duplicate_points;
				}
			}

			if (valid)
			{
				for (const auto& inner : polygon.inners())
				{
					if (detail::HasSamePoints(inner.data(), inner.size()))
					{
						valid = false;
						failure = boost::geometry::failure_duplicate_points;
					}
				}
			}

			// OK
			if (valid)
			{
				return{ Polygon(pVertex, vertexSize, holes) };
			}
		}

		// dissolve
		using MultiCwOpenPolygon = boost::geometry::model::multi_polygon<CwOpenPolygon>;
		boost::geometry::correct(polygon);
		MultiCwOpenPolygon solvedPolygons;
		boost::geometry::dissolve(polygon, solvedPolygons);

		Array<Polygon> results;

		for (const auto& solvedPolygon : solvedPolygons)
		{
			Array<Array<Vec2>> retHoles;

			for (const auto& hole : solvedPolygon.inners())
			{
				retHoles.emplace_back(hole.begin(), hole.end());
			}

			if (Validate(solvedPolygon.outer(), retHoles) == PolygonFailureType::OK)
			{
				results.emplace_back(solvedPolygon.outer(), retHoles);
			}
		}

		return results;
	}

	void Polygon::_Formatter(FormatData& formatData, const Polygon& value)
	{
		std::stringstream ss;
		
		ss << value;
		
		formatData.string.append(Unicode::WidenAscii(ss.str()));
	}
}
